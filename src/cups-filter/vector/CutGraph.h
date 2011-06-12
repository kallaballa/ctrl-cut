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

class PointGraph : public adjacency_list<vecS, vecS, undirectedS, VertexGeomProperty, EdgeGeomProperty> {
public:
  boost::graph_traits<PointGraph>::edges_size_type edge_count;

  typedef typename boost::graph_traits<PointGraph>
    ::vertex_descriptor Vertex;
  typedef typename boost::graph_traits<PointGraph>
    ::edge_descriptor Edge;
  typedef typename boost::graph_traits<PointGraph>
      ::vertices_size_type v_size;

  typedef map<const Point, Vertex> PointVertexMap;

  PointGraph() : adjacency_list<vecS, vecS, undirectedS, VertexGeomProperty, EdgeGeomProperty>() , edge_count(0){}
  PointGraph(const PointGraph& graph) : adjacency_list<vecS, vecS, undirectedS, VertexGeomProperty, EdgeGeomProperty>(graph) , edge_count(0) {}
  PointGraph(v_size size) : adjacency_list<vecS, vecS, undirectedS, VertexGeomProperty, EdgeGeomProperty>(size) , edge_count(0){}

  inline const Point& get_point(const PointGraph::Vertex& v) {
    return *get(vertex_geom, *this)[v].get<0>();
  }

  inline const Segment* get_segment(const PointGraph::Edge& e) {
    return get(edge_geom, *this)[e].get<1>();
  }

  PointGraph::Vertex* findVertex(const Point &point);
  PointGraph::Vertex addVertex(const Point &point);
  void createEdge(const Segment& seg);
  static PointGraph& createPointGraph(SegmentList::const_iterator start, SegmentList::const_iterator end);

private:
  PointVertexMap points;
};


class SegmentGraph : public adjacency_list<vecS, vecS, undirectedS, VertexGeomProperty, EdgeGeomProperty> {
public:
  boost::graph_traits<PointGraph>::edges_size_type edge_count;

  typedef typename boost::graph_traits<PointGraph>
    ::vertex_descriptor Vertex;
  typedef typename boost::graph_traits<PointGraph>
    ::edge_descriptor Edge;
  typedef typename boost::graph_traits<PointGraph>
      ::vertices_size_type v_size;

  typedef map<const Segment, Vertex> SegmentVertexMap;

  SegmentGraph() : adjacency_list<vecS, vecS, undirectedS, VertexGeomProperty, EdgeGeomProperty>() , edge_count(0){}
  SegmentGraph(const SegmentGraph& graph) : adjacency_list<vecS, vecS, undirectedS, VertexGeomProperty, EdgeGeomProperty>(graph) , edge_count(0) {}
  SegmentGraph(v_size size) : adjacency_list<vecS, vecS, undirectedS, VertexGeomProperty, EdgeGeomProperty>(size) , edge_count(0){}

  const Segment* getSegment(const SegmentGraph::Vertex& v) {
    return get(vertex_geom, *this)[v].get<1>();
  }

  SegmentGraph::Vertex* findVertex(const Segment& seg);
  SegmentGraph::Vertex addVertex(const Segment& seg);
  void createEdges(const Segment& seg1, const Segment& seg2);

  static SegmentGraph& createSegmentGraph(SegmentList::const_iterator start, SegmentList::const_iterator end);
private:
  SegmentVertexMap segments;
};
#endif
