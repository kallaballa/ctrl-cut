/*
 * Ctrl-Cut - A laser cutter CUPS driver
 * Copyright (C) 2009-2010 Amir Hassan <amir@viel-zu.org> and Marius Kintel <marius@kintel.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#include "util/Logger.hpp"
#include "cut/graph/Traverse.hpp"
#include "cut/geom/SegmentTree.hpp"
typedef SegmentGraphImpl<boost::setS, boost::setS> UniqueSegmentGraph;
typedef UniqueSegmentGraph::Vertex Vertex;
typedef UniqueSegmentGraph::Edge Edge;

void walkTheEdge(Route& skins, SegmentGraph& graph, const Edge lastEdge, const Vertex outV)
{
  Edge nextEdge;
  Segment& lastSegment = graph[lastEdge];
  Point& lastPoint = graph[outV];
  float lastSlope;

  if(lastPoint == lastSegment[0])
    lastSlope = lastSegment.getSlope();
  else if(lastPoint == lastSegment[1])
    lastSlope = lastSegment.getSlope(true);
  else
    assert(false);

  float minSlopeDiff = (CC_PI * 2);
  double slopeDiff;
  float candidateSlope;
  bool found = false;

  boost::graph_traits<SegmentGraph>::out_edge_iterator eit, eend;

  for(boost::tie(eit, eend) = boost::out_edges(outV, graph); eit != eend; ++eit) {
    const SegmentGraph::Edge candidate = *eit;
    Segment& segment = graph[candidate];

    //skip identical segment
    if(segment == lastSegment)
      continue;

    if(segment[0] != lastPoint)
      candidateSlope = segment.getSlope(true);
    else
      candidateSlope = segment.getSlope();

    slopeDiff = lastSlope - candidateSlope;

    if(slopeDiff < 0.01)
      slopeDiff += CC_PI * 2;

    /*std::cerr << *segment << std::endl;
    std::cerr << candidateSlope << std::endl;*/

    //save the edge with the least clockwise angle (= least slop_diff)
    if (slopeDiff < minSlopeDiff) {
      minSlopeDiff = slopeDiff;
      nextEdge = candidate;
      found = true;
    }
  }

  if (found) {
    Segment& seg = g[nextEdge];
    if(!skins.append(seg))
      skins.push_back(seg);
    walkTheEdge(skins, graph, nextEdge, get_opposite(graph, nextEdge, outV));
  }

  //polyline fully consumed
}

const Edge find_steapest(const Vertex& v, const UniqueSegmentGraph& g) {
  // Find next clockwise segment
  std::pair<UniqueSegmentGraph::edge_iterator, UniqueSegmentGraph::edge_iterator> edge_iterators = boost::out_edges(v, g);
  float steapest = 2 * CC_PI;
  const Edge* found = NULL;

  BOOST_FOREACH (Edge candidate, edge_iterators){
    const Segment* seg = g[candidate];
    float steapness = CC_PI - seg->getSlope();

    if(steapness < 0)
      steapness += (2 * CC_PI);

    if (steapness < steapest) {
      steapest = steapness;
      found = &candidate;
    }
  }
  assert(found != NULL);
  return *found;
}

void traverse_onion(Route& src, Route& skins)
{
  LOG_INFO_STR("Deonion");
  std::list<Point> most_left;

  struct more_left: public std::binary_function<Point, Point, bool> {
    bool operator()(const Point& __x, const Point& __y) const {
      return __x.x < __y.x;
    }
  };

  UniqueSegmentGraph g;
  BOOST_FOREACH(const Segment& seg, segments(src)) {
    g.add(seg);
  }

  BOOST_FOREACH(Vertex v, vertices(g)) {
    most_left.push_back(g[v]);
  }
  std::sort(g.index.begin(),g.index.end(), more_left);

  Vertex startVertex;
  while (!most_left.empty()) {
    const Point& most_left_point = most_left.front();
    const Vertex& most_left_vertex = g.index[most_left_point];

    const Edge& steapest = find_steapest(g,v);

    const Vertex next_vertex = get_opposite(g,steapest, most_left_vertex);
    const Point& next_point = g[next_vertex];

    if(next_point.y < most_left_point.y || (next_point.y == most_left_point.y && next_point.x > most_left_point.x))
      walkTheEdge(skins, g, steapest, next_vertex);
    else
      walkTheEdge(skins, g, steapest, most_left_vertex);
  }

  reverse(skins.begin(), skins.begin());
}
