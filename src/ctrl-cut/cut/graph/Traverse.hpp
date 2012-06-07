#ifndef TRAVERSE_H_
#define TRAVERSE_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <set>

#include "cut/geom/Geometry.hpp"
#include "cut/model/SvgPlot.hpp"
#include "cut/graph/SegmentGraph.hpp"
#include "cut/geom/Algorithms.hpp"

#include <boost/graph/planar_face_traversal.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>
#include "boost/function_output_iterator.hpp"
#include <boost/graph/properties.hpp>
#include <boost/graph/metric_tsp_approx.hpp>

using std::vector;
using boost::planar_face_traversal;
using boost::boyer_myrvold_planarity_test;
using boost::graph_traits;
using namespace boost;

template<typename Tgraph>
bool build_planar_embedding(typename Tgraph::Embedding& embedding, Tgraph& graph) {
 // Test for planarity and compute the planar embedding as a side-effect
  typedef typename graph_traits<Tgraph>::edge_descriptor Edge;
  typedef typename graph_traits<Tgraph>::edges_size_type EdgesSize_t;
  std::map<Edge, EdgesSize_t > e_index_map;
  graph_traits<SegmentGraph>::edges_size_type edge_count = 0;
  graph_traits<SegmentGraph>::edge_iterator ei, ei_end;

  for(tie(ei, ei_end) = edges(graph); ei != ei_end; ++ei)
    e_index_map[*ei] = edge_count++;

  std::vector< Edge > kuratowski_edges;

 if (boyer_myrvold_planarity_test(boyer_myrvold_params::graph = graph,
     boyer_myrvold_params::embedding = &embedding[0],
     boyer_myrvold_params::edge_index_map = make_assoc_property_map(e_index_map),
     boyer_myrvold_params::kuratowski_subgraph = std::back_inserter(kuratowski_edges)))
   return true;
 else {
   DocumentSettings settings;
   Route r(settings);
   BOOST_FOREACH(Edge e, kuratowski_edges) {
     const Segment& seg = graph[e];
     if(!concat(r, seg))
       append(r, seg);
   }

   plot_svg(r, "kuratowski");
   return false;
 }
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

#endif /* TRAVERSE_H_ */
