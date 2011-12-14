#include "SegmentGraph.h"
#include <iostream>
#include <vector>

void create_planar_graph(SegmentGraph& graph, SegmentList::const_iterator start, SegmentList::const_iterator end) {
    for (SegmentList::const_iterator it = start; it != end; ++it)
     graph.createEdge(*(*it));
}

bool SegmentGraph::hasEdge(const Vertex& in, const Vertex& out) {
  return boost::edge(in, out, *this).second;
}

void SegmentGraph::createEdge(const Segment& seg) {
  SegmentGraph::Vertex inV = addVertex(seg.first);
  SegmentGraph::Vertex outV = addVertex(seg.second);

  if(hasEdge(inV,outV))
    return;

  add_edge(inV, outV, SegmentProperty(&seg, 0), *this);
}

SegmentGraph::Vertex* SegmentGraph::findVertex(const Point& p) {
  PointMap::const_iterator it = points.find(PointProperty(&p));
  if (it == points.end())
    return NULL;
  else
    return (SegmentGraph::Vertex*)&(*it).second;
}

SegmentGraph::Vertex SegmentGraph::addVertex(const Point& p) {
  Vertex* v = findVertex(p);
  if (v == NULL) {
    PointProperty prop(&p);
    Vertex new_vertex = add_vertex(prop, *this);
    points[prop] = new_vertex;
    return new_vertex;
  }

  return *v;
}

