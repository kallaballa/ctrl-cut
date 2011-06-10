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
#include "Geometry.h"

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
  boost::graph_traits<CutGraph>::edges_size_type edge_count;

  typedef typename boost::graph_traits<CutGraph>
    ::vertex_descriptor Vertex;
  typedef typename boost::graph_traits<CutGraph>
    ::edge_descriptor Edge;
  typedef typename boost::graph_traits<CutGraph>
      ::vertices_size_type v_size;

  typedef map<const Point, Vertex> PointMap;

  CutGraph() : adjacency_list<vecS, vecS, undirectedS, PointProperty, GeomProperty>() , edge_count(0){}
  CutGraph(const CutGraph& graph) : adjacency_list<vecS, vecS, undirectedS, PointProperty, GeomProperty>(graph) , edge_count(0) {}
  CutGraph(v_size size) : adjacency_list<vecS, vecS, undirectedS, PointProperty, GeomProperty>(size) , edge_count(0){}

  CutGraph::Vertex* findVertex(const Point &point);
  CutGraph::Vertex addVertex(const Point &point);
  void createEdge(const Segment& seg);
  void createEdge(const SegmentString& string);
  void printPoints();
  static CutGraph& createCutGraph(SegmentList::const_iterator start, SegmentList::const_iterator end);
  static CutGraph& createCutGraph(StringList::const_iterator start, StringList::const_iterator end);

private:
  PointMap points;

};

inline const Point& get_point(const CutGraph::Vertex& v, const CutGraph& graph) {
  return get(vertex_point, graph)[v];
}

inline const Segment* get_segment(const CutGraph::Edge& e, const CutGraph& graph) {
  return get(edge_geom, graph)[e];
}
inline const SegmentString* get_segment_string(const CutGraph::Edge& e, const CutGraph& graph) {
  return get(edge_string, graph)[e];
}

inline void put_segment_string(const CutGraph::Edge& e, const SegmentString& string, CutGraph& graph) {
  put(get(edge_string, graph),e, &string);
}

#endif
