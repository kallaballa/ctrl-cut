/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */


#include "util/Logger.hpp"
#include "cut/graph/Traverse.hpp"
#include "cut/geom/SegmentTree.hpp"
#include "cut/geom/Route.hpp"
#include "cut/geom/algorithms/Algorithms.hpp"

typedef SegmentGraphImpl<boost::setS, boost::setS> UniqueSegmentGraph;
typedef UniqueSegmentGraph::Vertex Vertex;
typedef UniqueSegmentGraph::Edge Edge;
typedef std::map<Point,Vertex>::iterator IndexIter;

void walkTheEdge(Route& skins, UniqueSegmentGraph& graph, const Edge lastEdge, const Vertex outV)
{
  Edge nextEdge;
  Segment lastSegment = graph[lastEdge];
  Point lastPoint = graph[outV];

  graph.remove_edge(lastEdge);
  float lastSlope;

  if(lastPoint == lastSegment[0])
    lastSlope = lastSegment.getSlope();
  else if(lastPoint == lastSegment[1])
    lastSlope = lastSegment.getSlope(true);
  else
    assert(false);

  float minSlopeDiff = (CC_PI * 2);
  float candidateSlope;
  double slopeDiff;
  bool found = false;

  BOOST_FOREACH(Edge candidate , boost::out_edges(outV, graph)) {
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
    Segment& seg = graph[nextEdge];
    if(!concat(skins, seg))
      append(skins,seg);

    graph.index.erase(seg.first);
    graph.index.erase(seg.second);
    walkTheEdge(skins, graph, nextEdge, get_opposite(graph, nextEdge, outV));
  }

  //polyline fully consumed
}

bool find_steapest(Edge& found, const Vertex& v, const UniqueSegmentGraph& g) {
  // Find next clockwise segment
  float steapest = 2 * CC_PI;
  bool f = false;

  BOOST_FOREACH (const Edge candidate, boost::out_edges(v, g)){
    const Segment& seg = g[candidate];
    float steapness = CC_PI - seg.getSlope();

    if(steapness < 0)
      steapness += (2 * CC_PI);

    if (steapness < steapest) {
      steapest = steapness;
      found = candidate;
      f=true;
    }
  }
  return f;
}

void traverse_onion(Route& src, Route& skins)
{
  LOG_INFO_STR("Deonion");

  UniqueSegmentGraph g;
  for(const SegmentPtr seg : segments(src)) {
    g.addSegment(*seg.get());
  }

  // points are sorted first by x then by y. therefore the
  // first point in the index must be the most left
  while (!g.index.empty()) {
    Point most_left_point = (*g.index.begin()).first;
    Vertex most_left_vertex = (*g.index.begin()).second;

    Edge steapest;

    if(!find_steapest(steapest, most_left_vertex, g)) {
      //dangling vertex
      g.index.erase(g.index.begin());
      continue;
    }

    const Vertex next_vertex = get_opposite(g,steapest, most_left_vertex);
    const Point& next_point = g[next_vertex];

    const Segment& steapest_seg = g[steapest];
    if(!concat(skins, steapest_seg))
      append(skins,steapest_seg);

    g.index.erase(g.index.begin());

    if(next_point.y < most_left_point.y || (next_point.y == most_left_point.y && next_point.x > most_left_point.x)) {
      walkTheEdge(skins, g, steapest, next_vertex);
    }
    else {
      walkTheEdge(skins, g, steapest, most_left_vertex);
    }

  }

  reverse(skins.begin(), skins.end());
}
