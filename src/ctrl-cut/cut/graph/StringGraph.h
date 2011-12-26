#ifndef STRINGGRAPH_H_
#define STRINGGRAPH_H_

#include <iostream>
#include <utility>
#include <algorithm>
#include <vector>
#include <map>
#include <string>

#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include "cut/geom/Route.h"

using boost::adjacency_list;
using boost::undirectedS;
using boost::vecS;
using boost::add_vertex;
using boost::add_edge;
using std::vector;
using std::map;
using std::pair;

struct StringProperty {
  double weight;

  StringProperty(double weight=0) : weight(weight) {}
/*
  bool operator<(const StringProperty& other) const {
    return this->string < other.string;
  }*/
};

class StringGraph : public adjacency_list<vecS, vecS, undirectedS, Point, StringProperty> {
public:

  boost::graph_traits<StringGraph>::edges_size_type edge_count;

  typedef boost::graph_traits<StringGraph>
    ::vertex_descriptor Vertex;
  typedef boost::graph_traits<StringGraph>
    ::edge_descriptor Edge;
  typedef boost::graph_traits<StringGraph>
      ::vertices_size_type v_size;
    typedef std::vector<std::vector< StringGraph::Edge > > Embedding;

  typedef map<const Point, Vertex> PointMap;

  StringGraph() : adjacency_list<vecS, vecS, undirectedS, Point, StringProperty>() , edge_count(0){}
  StringGraph(const StringGraph& graph) : adjacency_list<vecS, vecS, undirectedS, Point, StringProperty>(graph) , edge_count(0) {}
  StringGraph(v_size size) : adjacency_list<vecS, vecS, undirectedS, Point, StringProperty>(size) , edge_count(0){}

  bool findVertex(StringGraph::Vertex& v, const Point& map);
  StringGraph::Vertex addVertex(const Point& p);
  void createWorkEdge(const Vertex& in, const Vertex& out);
  void createMoveEdge(const Vertex& in, const Vertex& out);
  bool hasEdge(const Vertex& in, const Vertex& out);
  void permutateEdges(SegmentString& string, Vertex v_origin, vector<Vertex>& outVertices);
private:
  PointMap points;
};

StringGraph::Vertex create_complete_graph_from_point(StringGraph& graph, const Point& origin, Route::StringIter start, Route::StringIter end);

#endif
