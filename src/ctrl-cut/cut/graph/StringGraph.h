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

struct WeightProperty {
  double weight;

  WeightProperty(double weight=0) : weight(weight) {}
};

class DistanceGraph : public adjacency_list<vecS, vecS, undirectedS, Point, WeightProperty> {
public:
  boost::graph_traits<DistanceGraph>::edges_size_type edge_count;

  typedef boost::graph_traits<DistanceGraph>
    ::vertex_descriptor Vertex;
  typedef boost::graph_traits<DistanceGraph>
    ::edge_descriptor Edge;
  typedef boost::graph_traits<DistanceGraph>
      ::vertices_size_type v_size;
    typedef std::vector<std::vector< DistanceGraph::Edge > > Embedding;

  typedef map<const Point, Vertex> PointMap;

  DistanceGraph() : adjacency_list<vecS, vecS, undirectedS, Point, WeightProperty>() , edge_count(0){}
  DistanceGraph(const DistanceGraph& graph) : adjacency_list<vecS, vecS, undirectedS, Point, WeightProperty>(graph) , edge_count(0) {}
  DistanceGraph(v_size size) : adjacency_list<vecS, vecS, undirectedS, Point, WeightProperty>(size) , edge_count(0){}

  bool findVertex(DistanceGraph::Vertex& v, const Point& map);
  DistanceGraph::Vertex addVertex(const Point& p);
  void createWorkEdge(const Vertex& in, const Vertex& out);
  void createMoveEdge(const Vertex& in, const Vertex& out);
  bool hasEdge(const Vertex& in, const Vertex& out);
  void permutateEdges(const Point& front, const Point& back, Vertex v_origin, vector<Vertex>& outVertices);
private:
  PointMap points;
};

DistanceGraph::Vertex create_complete_graph_from_point(DistanceGraph& graph, const Point& origin, Route::iterator start, Route::iterator end);

#endif
