#ifndef TRAVERSE_H_
#define TRAVERSE_H_

#include <iostream>
#include <vector>

#include <boost/graph/planar_face_traversal.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>

#include "Geometry.h"
#include "CutEdge.h"

using std::vector;
using boost::planar_face_traversal;
using boost::boyer_myrvold_planarity_test;
using boost::graph_traits;
using namespace boost;

void make_linestrings(StringList& strings, SegmentList::iterator first, SegmentList::iterator last);
void travel_linestrings(StringList& strings, StringList::iterator first, StringList::iterator last);
bool build_planar_embedding(CutGraph::Embedding& embedding, CutGraph& graph);

template<typename Visitor>
inline void traverse_planar_faces(CutGraph& graph, Visitor& visitor) {
  CutGraph::Embedding embedding(num_vertices(graph));
  assert(build_planar_embedding(embedding, graph));
  planar_face_traversal(graph, &embedding[0], visitor);
}

#endif /* TRAVERSE_H_ */
