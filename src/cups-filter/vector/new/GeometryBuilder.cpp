#include "GeometryBuilder.h"
#include <boost/ref.hpp>
#include <boost/graph/properties.hpp>
#include <vector>


using std::vector;
using boost::planar_face_traversal;
using boost::boyer_myrvold_planarity_test;
using boost::graph_traits;


void GeometryBuilder::build() {
  using namespace boost;
  property_map<UndirectedGraph, edge_index_t>::type e_index = get(edge_index, this->graph);
  graph_traits<UndirectedGraph>::edges_size_type edge_count = 0;
  graph_traits<UndirectedGraph>::edge_iterator ei, ei_end;
  for(tie(ei, ei_end) = edges(this->graph); ei != ei_end; ++ei)
    put(e_index, *ei, edge_count++);

  // Test for planarity and compute the planar embedding as a side-effect
  typedef std::vector< UndirectedEdge > vec_t;
  std::vector<vec_t> embedding(num_vertices(this->graph));
  if (boyer_myrvold_planarity_test(boyer_myrvold_params::graph = this->graph,
                                   boyer_myrvold_params::embedding =
                                       &embedding[0]
                                   )
      )
    std::cerr << "Input graph is planar" << std::endl;
  else
    std::cerr << "Input graph is not planar" << std::endl;


  output_visitor vis;
 planar_face_traversal(this->graph, &embedding[0], vis);
}
