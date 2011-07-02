#ifndef TRAVERSE_H_
#define TRAVERSE_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <set>

#include "vector/graph/CutGraph.h"
#include <boost/graph/planar_face_traversal.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>

#include "vector/geom/Geometry.h"


using std::vector;
using boost::planar_face_traversal;
using boost::boyer_myrvold_planarity_test;
using boost::graph_traits;
using namespace boost;

void dump_linestrings(const std::string &filename, StringList::iterator first, StringList::iterator last);
void dump_linestrings(std::ostream& os, StringList::iterator first, StringList::iterator last);
void make_linestrings(StringList& strings, SegmentList::iterator first, SegmentList::iterator last, SegmentGraph& graph);
void make_linestrings(StringList& strings, SegmentList::iterator first, SegmentList::iterator last);
void travel_linestrings(StringList& strings, StringList::iterator first, StringList::iterator last);

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

#endif /* TRAVERSE_H_ */
