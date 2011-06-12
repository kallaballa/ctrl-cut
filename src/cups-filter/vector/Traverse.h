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

template<typename Visitor>
inline void traverse_planar_faces(PointGraph& graph, Visitor& visitor) {
  // Test for planarity and compute the planar embedding as a side-effect
  typedef std::vector< PointGraph::Edge > vec_t;
  std::vector<vec_t> embedding(num_vertices(graph));

  if (boyer_myrvold_planarity_test(boyer_myrvold_params::graph = graph,
      boyer_myrvold_params::embedding = &embedding[0]))
    std::cerr<< "Input graph is planar" << std::endl;
  else
    std::cerr << "Input graph is not planar" << std::endl;

  planar_face_traversal(graph, &embedding[0], visitor);
}

#endif /* TRAVERSE_H_ */
