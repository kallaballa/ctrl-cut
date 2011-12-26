#include "SegmentGraph.h"
#include <iostream>
#include <vector>

void create_planar_graph(SegmentGraph& graph, SegmentList::const_iterator start, SegmentList::const_iterator end) {
    for (SegmentList::const_iterator it = start; it != end; ++it)
     graph.createEdge((*it));
}

bool SegmentGraph::hasEdge(const Vertex& in, const Vertex& out) {
  return boost::edge(in, out, *this).second;
}

void SegmentGraph::createEdge(const Segment& seg) {
  SegmentGraph::Vertex inV = addVertex(seg.first);
  SegmentGraph::Vertex outV = addVertex(seg.second);

  if(hasEdge(inV,outV))
    return;

  add_edge(inV, outV, SegmentProperty(seg), *this);
}

bool SegmentGraph::findVertex(Vertex& v, const Point&  p) {
  PointMap::const_iterator it = points.find(p);
  if (it == points.end())
    return false;

  v=(*it).second;
  return true;
}

SegmentGraph::Vertex SegmentGraph::addVertex(const Point&  p) {
  Vertex v;
  if (!findVertex(v,p)) {
    Vertex new_vertex = add_vertex(PointProperty(p),*this);
    points[p] = new_vertex;
    return new_vertex;
  }

  return v;
}

