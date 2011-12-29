#ifndef TRAVERSE_H_
#define TRAVERSE_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <set>

#include "Deonion.h"
#include "cut/graph/CutGraph.h"

#include <boost/graph/planar_face_traversal.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>

#include "cut/geom/Geometry.h"


using std::vector;
using boost::planar_face_traversal;
using boost::boyer_myrvold_planarity_test;
using boost::graph_traits;
using namespace boost;

template<typename Graph>
class RouteBuilder {
public:
  Graph* graph;
  Route* route;

  RouteBuilder(Graph& graph, Route& route) : graph(&graph), route(&route), first(true) {}
  RouteBuilder(const RouteBuilder& rb) : graph(rb.graph), route(rb.route), first(rb.first) {}

  void operator()(typename Graph::Vertex v) {
    Point& current = (*graph)[v];
    if(!first) {
      route->append(Segment(this->last,current));
    }
    this->last = current;
  }
private:
  bool first;
  Point last;
};

void dump(const std::string &filename, CutModel::iterator first, CutModel::iterator last);
void dump(std::ostream& os,CutModel::iterator first, CutModel::iterator last);
/*void make_linestrings(Route& strings, SegmentList::const_iterator first, SegmentList::const_iterator last, SegmentGraph& graph);
void make_linestrings(Route& strings, SegmentList::const_iterator first, SegmentList::const_iterator last);*/
void travel_linestrings(Route& route, Route::iterator first, Route::iterator last);

template<typename Graph>
bool build_planar_embedding(typename Graph::Embedding& embedding, Graph& graph) {
 // Test for planarity and compute the planar embedding as a side-effect
 if (boyer_myrvold_planarity_test(boyer_myrvold_params::graph = graph,
     boyer_myrvold_params::embedding = &embedding[0]))
   return true;
 else
   return false;
}

template<typename Visitor>
inline void traverse_planar_faces(SegmentGraph& graph, Visitor& visitor) {
  SegmentGraph::Embedding Embedding;

  std::map<SegmentGraph::Edge, graph_traits<SegmentGraph>::edges_size_type > e_index_map;
  graph_traits<SegmentGraph>::edges_size_type edge_count = 0;
  graph_traits<SegmentGraph>::edge_iterator ei, ei_end;

  for(tie(ei, ei_end) = edges(graph); ei != ei_end; ++ei)
    e_index_map[*ei] = edge_count++;

  SegmentGraph::Embedding embedding(num_vertices(graph));
  assert(build_planar_embedding(embedding, graph));
  planar_face_traversal(graph, &embedding[0], visitor,make_assoc_property_map(e_index_map));
}

template<typename Graph>
struct VertexPair {
  typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
  Vertex lower;
  Vertex higher;
  VertexPair() {

  }
  VertexPair(Vertex v1, Vertex v2) {
    if(v1 == v2)
      assert(!"source and target vertex are equal");

    if(v1 < v2) {
      this->lower = v1;
      this->higher = v2;
    } else {
      this->lower = v2;
      this->higher = v1;
    }
  }

  bool operator<(const VertexPair& other) const {
    return this->lower < other.lower || (this->lower == other.lower && this->higher < other.higher);
  }
};

template<typename Graph>
inline bool is_complete_graph(Graph& graph) {
  typename graph_traits<Graph>::edge_iterator ei, ei_end;
  typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
  typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
  typedef VertexPair<Graph> VPair;
  std::set<VPair > uniqVertexPairs;

  for(boost::tie(ei, ei_end) = edges(graph); ei != ei_end; ++ei) {
    Edge edge = *ei;
    VPair vpair(source(edge, graph), target(edge, graph));
    if(!uniqVertexPairs.insert(vpair).second) {
      assert(!"duplicate edge found");
    }
  }

  typename graph_traits<Graph>::vertex_iterator vi, vi_end;
  typename graph_traits<Graph>::vertex_iterator vj, vj_end;
  Vertex i,j;

  for(boost::tie(vi, vi_end) = vertices(graph); vi != vi_end; ++vi) {
    i = *vi;
    for(boost::tie(vj, vj_end) = vertices(graph); vj != vj_end; ++vj) {
      j = *vj;
      if(i == j)
        continue;

      if(!boost::edge(i, j,graph).second)
        return false;
    }
  }

  return true;
}

#endif /* TRAVERSE_H_ */
