#include "CutGraph.h"

#include <iostream>
#include <vector>

using std::vector;

boost::graph_traits<CutGraph>::edges_size_type CutGraph::edge_count = 0;

CutGraph::Vertex CutGraph::findOrAddVertex(const Point &point) {
  PointMap::const_iterator it = points.find(point);
    if (it == points.end()) {
        Vertex new_vertex = add_vertex(PointProperty(point),*this);
        points[point] = new_vertex;
        return new_vertex;
    }

    return (*it).second;
}

void CutGraph::createEdge(const Segment& seg) {
  CutGraph::Vertex inV = findOrAddVertex(seg.first);
  CutGraph::Vertex outV = findOrAddVertex(seg.second);

  GeomProperty geomProp(&seg, StringProperty(0, IndexProperty(edge_count++)));
  add_edge(inV, outV, geomProp, *this);
}
