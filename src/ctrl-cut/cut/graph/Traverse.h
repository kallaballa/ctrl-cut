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
#include "boost/function_output_iterator.hpp"
#include <boost/graph/properties.hpp>
#include <boost/graph/metric_tsp_approx.hpp>

#include "cut/geom/Geometry.h"


using std::vector;
using boost::planar_face_traversal;
using boost::boyer_myrvold_planarity_test;
using boost::graph_traits;
using namespace boost;

template<typename Tgraph, typename TsegmentOutputIterator>
class VertexAppender {
public:
  Tgraph* graph;
  TsegmentOutputIterator* sink;

  VertexAppender(Tgraph& graph, TsegmentOutputIterator& sink) : graph(&graph), sink(&sink), first(true) {
    BOOST_CONCEPT_ASSERT((SegmentOutputIterator<TsegmentOutputIterator>));
  }
  VertexAppender(const VertexAppender& rb) : graph(rb.graph), sink(rb.sink), first(rb.first) {
    BOOST_CONCEPT_ASSERT((SegmentOutputIterator<TsegmentOutputIterator>));
  }

  void operator()(typename Tgraph::Vertex v) {
    const Point& current = (*graph)[v];
    if(!first) {
      sink++ = Segment(this->last,current);
    }
    this->last = current;
  }
private:
  bool first;
  Point last;
};

template<typename TsegmentInputIterator>
void check(TsegmentInputIterator first, TsegmentInputIterator last) {
  BOOST_CONCEPT_ASSERT((SegmentInputIterator<TsegmentInputIterator>));

  std::set<Segment> uniq_segments;

  for (TsegmentInputIterator it_s = first; it_s != last; ++it_s) {
    const Segment& seg = *it_s;
    // assert the segments are globally unique
    assert(uniq_segments.find(seg) == uniq_segments.end() && uniq_segments.find(Segment(seg.second, seg.first)) == uniq_segments.end());
    uniq_segments.insert(seg);
  }
}

void dump(const std::string &filename, Route::iterator first, Route::iterator last);
void dump(std::ostream& os,Route::iterator first, Route::iterator last);
/*void make_linestrings(Route& strings, SegmentList::const_iterator first, SegmentList::const_iterator last, SegmentGraph& graph);
void make_linestrings(Route& strings, SegmentList::const_iterator first, SegmentList::const_iterator last);*/


template<typename TsegmentOutputIterator, typename TsegmentInputIterator>
void travel_linestrings(TsegmentOutputIterator sink, TsegmentInputIterator begin, TsegmentInputIterator end) {
  BOOST_CONCEPT_ASSERT((SegmentInputIterator<TsegmentInputIterator>));
  BOOST_CONCEPT_ASSERT((SegmentOutputIterator<TsegmentOutputIterator>));
  using namespace boost;
  LOG_INFO_STR("travel linestrings");

  DistanceGraph graph;
  DistanceGraph::Vertex v_origin = create_complete_graph_from_point(graph, Point(0,0),begin, end);

  typedef boost::property_map<boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, Point, WeightProperty>, double WeightProperty::*>::type WeightMap;
  WeightMap weight_map(get(&WeightProperty::weight, graph));

  vector<DistanceGraph::Vertex> tour;
  double len = 0.0;
  VertexAppender<DistanceGraph, TsegmentOutputIterator> rb(graph, sink);

  boost::metric_tsp_approx_from_vertex(graph, v_origin, weight_map,
      boost::make_tsp_tour_visitor(
          make_function_output_iterator(boost::bind<void>(rb, _1))
      )
  );

  LOG_INFO_MSG("Tour length", len);

#ifdef DEBUG
  check(begin, end);
#endif
}

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
