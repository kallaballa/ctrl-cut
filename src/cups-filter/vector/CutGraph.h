#ifndef CUTGRAPH_H_
#define CUTGRAPH_H_

#include <iostream>
#include <utility>
#include <algorithm>
#include <vector>
#include <map>
#include <string>

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

class CutGraph : public adjacency_list<vecS, vecS, undirectedS, VertexGeometry, EdgeGeometry> {
public:
  boost::graph_traits<CutGraph>::edges_size_type edge_count;

  typedef boost::graph_traits<CutGraph>
    ::vertex_descriptor Vertex;
  typedef boost::graph_traits<CutGraph>
    ::edge_descriptor Edge;
  typedef boost::graph_traits<CutGraph>
      ::vertices_size_type v_size;
    typedef std::vector<std::vector< CutGraph::Edge > > Embedding;

  typedef map<const VertexGeometry, Vertex> GeomVertexMap;

  CutGraph() : adjacency_list<vecS, vecS, undirectedS, VertexGeometry, EdgeGeometry>() , edge_count(0){}
  CutGraph(const CutGraph& graph) : adjacency_list<vecS, vecS, undirectedS, VertexGeometry, EdgeGeometry>(graph) , edge_count(0) {}
  CutGraph(v_size size) : adjacency_list<vecS, vecS, undirectedS, VertexGeometry, EdgeGeometry>(size) , edge_count(0){}

  inline const Point* getPoint(const CutGraph::Vertex& v) {
    return (*this)[v].point;
  }

  inline const Segment* getSegment(const CutGraph::Vertex& v) {
    return (*this)[v].segment;
  }

  inline const SegmentString* getSegmentString(const CutGraph::Vertex& v) {
    return (*this)[v].string;
  }

  inline const Point* getPoint(const CutGraph::Edge& e) {
    return (*this)[e].point;
  }

  inline const Segment* getSegment(const CutGraph::Edge& e) {
    return (*this)[e].segment;
  }

  inline const SegmentString* getSegmentString(const CutGraph::Edge& e) {
    return (*this)[e].string;
  }

  inline void setSegmentString(const CutGraph::Edge& e, const SegmentString& string) {
    (*this)[e].string = &string;
  }

  CutGraph::Vertex* findVertex(const VertexGeometry &map);
  CutGraph::Vertex addVertex(const VertexGeometry &map);
  void createEdge(const Segment& seg);
  void createEdges(const SegmentString& string1, const SegmentString& string2);
  void createEdges(const SegmentString& string1, const SegmentString& string2, const CutGraph::Vertex& v_origin);
  void createMetricEdge(const Vertex& in, const Vertex& out, EdgeGeometry map);
  bool hasEdge(const Vertex& in, const Vertex& out);
  void permutateEdges(const SegmentString& string, Vertex v_origin, vector<Vertex>& outVertices);

private:
  GeomVertexMap geometries;
};

void dump_graph(CutGraph& graph, const std::string& filename);
void create_segment_graph(CutGraph& graph, SegmentList::const_iterator start, SegmentList::const_iterator end);
void create_complete_graph(CutGraph& graph, StringList::const_iterator start, StringList::const_iterator end);
CutGraph::Vertex create_complete_graph_from_point(CutGraph& graph, const Point& origin, StringList::const_iterator start, StringList::const_iterator end);

#endif
