#include "CutGraph.h"

#include <iostream>
#include <vector>

using std::vector;

PointGraph& PointGraph::createPointGraph(SegmentList::const_iterator start, SegmentList::const_iterator end) {
  PointGraph& graph = *new PointGraph();

  for (SegmentList::const_iterator it = start; it != end; ++it)
    graph.createEdge(*(*it));

  return graph;
}

void PointGraph::createEdge(const Segment& seg) {
  PointGraph::Vertex inV = addVertex(seg.first);
  PointGraph::Vertex outV = addVertex(seg.second);

  //FIXME do we want to filter duplicated edges?
  if(boost::edge(inV,outV, *this).second)
    return;

  EdgeGeomProperty geomProp(*new GeometryMapping(0, &seg, 0), IndexProperty(edge_count++, WeightProperty(0)));
  add_edge(inV, outV, geomProp, *this);
}

PointGraph::Vertex* PointGraph::findVertex(const Point &point) {
  PointVertexMap::const_iterator it = points.find(point);
  if (it == points.end())
    return NULL;
  else
    return (PointGraph::Vertex*)&(*it).second;
}

PointGraph::Vertex PointGraph::addVertex(const Point &point) {
  Vertex* v = findVertex(point);
  if (v == NULL) {
    Vertex new_vertex = add_vertex(* new VertexGeomProperty(*new GeometryMapping(&point,0,0)), *this);
    points[point] = new_vertex;
    return new_vertex;
  }

  return *v;
}
