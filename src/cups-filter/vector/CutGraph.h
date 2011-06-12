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

  typedef map<const GeometryMapping, Vertex> GeomVertexMap;

  PointGraph() : adjacency_list<vecS, vecS, undirectedS, VertexGeomProperty, EdgeGeomProperty>() , edge_count(0){}
  PointGraph(const PointGraph& graph) : adjacency_list<vecS, vecS, undirectedS, VertexGeomProperty, EdgeGeomProperty>(graph) , edge_count(0) {}
  PointGraph(v_size size) : adjacency_list<vecS, vecS, undirectedS, VertexGeomProperty, EdgeGeomProperty>(size) , edge_count(0){}

  inline const Point* gePoint(const PointGraph::Vertex& v) {
    return get(vertex_geom, *this)[v].get<0>();
  }

  inline const Segment* getSegment(const PointGraph::Vertex& v) {
    return get(vertex_geom, *this)[v].get<1>();
  }

  inline const SegmentString* getSegmentString(const PointGraph::Vertex& v) {
    return get(vertex_geom, *this)[v].get<2>();
  }

  inline const Point* gePoint(const PointGraph::Edge& e) {
    return get(edge_geom, *this)[e].get<0>();
  }

  inline const Segment* getSegment(const PointGraph::Edge& e) {
    return get(edge_geom, *this)[e].get<1>();
  }

  inline const SegmentString* getSegmentString(const PointGraph::Edge& e) {
    return get(edge_geom, *this)[e].get<2>();
  }

  inline const SegmentString* setSegmentString(const PointGraph::Edge& e, const SegmentString& string) {
    return get(edge_geom, *this)[e].get<2>() = &string;
  }

  PointGraph::Vertex* findVertex(const GeometryMapping &map);
  PointGraph::Vertex addVertex(const GeometryMapping &map);
  void createEdge(const Segment& seg);
  void createEdges(const SegmentString& string1, const SegmentString& string2, const PointGraph::Vertex& v_origin);
  static PointGraph& createPlanarGraph(SegmentList::const_iterator start, SegmentList::const_iterator end);
  static std::pair<PointGraph&, PointGraph::Vertex>& createCompleteGraph(StringList::const_iterator start, StringList::const_iterator end);

private:
  GeomVertexMap geometries;
};

#endif
