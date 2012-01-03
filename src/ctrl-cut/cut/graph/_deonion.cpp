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

#include "Deonion.hpp"
#include "util/Logger.hpp"
#include "cut/graph/Traverse.hpp"
#include "cut/geom/SegmentTree.hpp"

void walkTheEdge(Route& skins, SegmentGraph& graph, const SegmentGraph::Edge lastEdge, const SegmentGraph::Vertex outV)
{
  SegmentGraph::Edge nextEdge;
  Segment& lastSegment = graph[lastEdge];
  Point& lastPoint = graph[outV];
  float lastSlope;

  if(lastPoint == lastSegment[0])
    lastSlope = lastSegment.getSlope();
  else if(lastPoint == lastSegment[1])
    lastSlope = lastSegment.getSlope(true);
  else
    assert(false);

  if(!graph[lastEdge].owned) {
    if(!skins.append(lastSegment))
      skins.push_back(lastSegment);
    graph[lastEdge].owned = true;
  } else {
    return;
  }

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
    walkTheEdge(skins, graph, nextEdge, get_opposite(graph, nextEdge, outV));
  }

  //polyline fully consumed
}

bool pop_mostleft_free_vertex(SegmentGraph::Vertex& v, SegmentGraph::PointMap& point2Vertex, const SegmentGraph& graph) {
  boost::graph_traits<SegmentGraph>::out_edge_iterator eit, eend;
  for(SegmentGraph::PointMap::iterator it = point2Vertex.begin(); it != point2Vertex.end(); ++it) {
    SegmentGraph::Vertex& candidate = (*it).second;
    point2Vertex.erase(it);

    boost::tie(eit, eend) = boost::out_edges(candidate, graph);
    if(eit != eend && !graph[*eit].owned) {
      // found vertex of a free edge
      v = candidate;
      return true;
    }
  }
  return false;
}

void traverse_onion(Route& skins, SegmentList::iterator first, SegmentList::iterator last)
{
  LOG_INFO_STR("Deonion");
  SegmentGraph graph;
  create_planar_graph(graph, first, last);
  SegmentGraph::PointMap point2Vertex = graph.getPointMap();
  boost::graph_traits<SegmentGraph>::out_edge_iterator eit, eend;
  SegmentGraph::Vertex startVertex;
  while (pop_mostleft_free_vertex(startVertex, point2Vertex, graph)) {
    const Point& startPoint = graph[startVertex];
    boost::tie(eit, eend) = boost::out_edges(startVertex, graph);
    SegmentGraph::Edge steapest;
    find_steapest(steapest, graph, eit, eend);

    const SegmentGraph::Vertex outVertex = get_opposite(graph,steapest, startVertex);
    const Point& outPoint = graph[outVertex];

    if(outPoint.y < startPoint.y || (outPoint.y == startPoint.y && outPoint.x > startPoint.x))
      walkTheEdge(skins, graph, steapest, outVertex);
    else
      walkTheEdge(skins, graph, steapest, startVertex);
  }

  reverse(skins.begin(), skins.begin());
}
