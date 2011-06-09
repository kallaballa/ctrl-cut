#ifndef CUTGRAPH_H_
#define CUTGRAPH_H_

#include <iostream>
#include <utility>
#include <algorithm>
#include <vector>
#include <map>

#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "CutEdge.h"
#include "CutVertex.h"

using boost::adjacency_list;
using boost::undirectedS;
using boost::vecS;
using boost::add_vertex;
using boost::add_edge;
using std::vector;
using std::map;
using std::pair;

class CutGraph : public adjacency_list<vecS, vecS, undirectedS, PointProperty, GeomProperty> {

public:
  typedef typename boost::graph_traits<CutGraph>
    ::vertex_descriptor Vertex;

  typedef typename boost::graph_traits<CutGraph>
    ::edge_descriptor Edge;

  typedef map<Point, Vertex> PointMap;

  CutGraph::Vertex findOrAddVertex(const Point &point);
  void createEdge(const Segment& seg);

private:
  static boost::graph_traits<CutGraph>::edges_size_type edge_count;
  PointMap points;
};

inline const Point& get_point(const CutGraph::Vertex& v, const CutGraph& graph) {
  return get(vertex_point, graph)[v];
}

inline const Segment* get_segment(const CutGraph::Edge& e, const CutGraph& graph) {
  return get(edge_geom, graph)[e];
}

#endif
