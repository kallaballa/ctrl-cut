#ifndef TRAVERSE_H_
#define TRAVERSE_H_

#include <iostream>
#include <fstream>
#include <vector>

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
void make_linestrings(StringList& strings, SegmentList::iterator first, SegmentList::iterator last, CutGraph& graph);
void make_linestrings(StringList& strings, SegmentList::iterator first, SegmentList::iterator last);
void travel_linestrings(StringList& strings, StringList::iterator first, StringList::iterator last);
bool build_planar_embedding(CutGraph::Embedding& embedding, CutGraph& graph);

template<typename Visitor>
inline void traverse_planar_faces(CutGraph& graph, Visitor& visitor) {
  std::map<CutGraph::Edge, graph_traits<CutGraph>::edges_size_type > e_index_map;
  graph_traits<CutGraph>::edges_size_type edge_count = 0;
  graph_traits<CutGraph>::edge_iterator ei, ei_end;

  for(tie(ei, ei_end) = edges(graph); ei != ei_end; ++ei)
    e_index_map[*ei] = edge_count++;

  CutGraph::Embedding embedding(num_vertices(graph));
  assert(build_planar_embedding(embedding, graph));
  planar_face_traversal(graph, &embedding[0], visitor,make_assoc_property_map(e_index_map));
}

#endif /* TRAVERSE_H_ */
