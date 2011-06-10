#include "CutGraph.h"

#include <iostream>
#include <vector>

using std::vector;

CutGraph& CutGraph::createCutGraph(SegmentList::const_iterator start, SegmentList::const_iterator end) {
  CutGraph& graph = (*new CutGraph());

  for (SegmentList::const_iterator it = start; it != end; ++it)
    graph.createEdge(*(*it));

  return graph;
}

CutGraph& CutGraph::createCutGraph(StringList::const_iterator start, StringList::const_iterator end) {
  CutGraph& graph = (*new CutGraph());

  for (StringList::const_iterator it = start; it != end; ++it)
    graph.createEdge(*(*it));

  return graph;
}

void CutGraph::createEdge(const Segment& seg) {
  CutGraph::Vertex inV = addVertex(seg.first);
  CutGraph::Vertex outV = addVertex(seg.second);
  GeomProperty geomProp(&seg, StringProperty(0, IndexProperty(edge_count++, WeightProperty(0))));
  add_edge(inV, outV, geomProp, *this);
}

void CutGraph::createEdge(const SegmentString& string) {
  CutGraph::Vertex inV = addVertex(*string.frontPoints());
  CutGraph::Vertex outV = addVertex(*string.backPoints());
  GeomProperty geomProp(0, StringProperty(&string, IndexProperty(edge_count++, WeightProperty(0))));
  add_edge(inV, outV, geomProp, *this);
}

CutGraph::Vertex* CutGraph::findVertex(const Point &point) {
  PointMap::const_iterator it = points.find(point);
  if (it == points.end())
    return NULL;
  else
    return (CutGraph::Vertex*)&(*it).second;
}

CutGraph::Vertex CutGraph::addVertex(const Point &point) {
  Vertex* v = findVertex(point);
  if (v == NULL) {
    Vertex new_vertex = add_vertex(PointProperty(point), *this);
    points[point] = new_vertex;
    return new_vertex;
  }

  return *v;
}

void CutGraph::printPoints() {
  for(PointMap::const_iterator it = points.begin(); it != points.end(); ++it) {
    std::cerr << (*it).first << std::endl;
  }
}
