#ifndef GEOMETRYBUILDER_H_
#define GEOMETRYBUILDER_H_

#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/ref.hpp>
#include <vector>

#include <boost/graph/planar_face_traversal.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>


#include "CutModel.h"

using namespace boost;

struct output_visitor : public planar_face_traversal_visitor
{
  void begin_face() { std::cout << "New face: "; }
  void end_face() { std::cout << std::endl; }
  template <typename Vertex>
  void next_vertex(Vertex v)
  {
    std::cout << v << " ";
  }

  template <typename Edge>
  void next_edge(Edge e)
  {
    std::cout << e << " ";
  }
};

/*
struct output_visitor: public planar_face_traversal_visitor {
  Graph& graph;
  const property_map<Graph, vertex_point_t>::type &vPoint;

  output_visitor(Graph& graph) :
    graph(graph), vPoint(get(vertex_point, graph)) {
  }

  void begin_face() {
    std::cout << "New face: ";
  }
  void end_face() {
    std::cout << std::endl;
  }
  template <typename Edge>
  void next_edge(Edge e) {
    std::cout << "{" << vPoint[source(e, this->graph)] << " -> "
        << vPoint[target(e, this->graph)] << "}" << std::endl;
  }
};*/

class GeometryBuilder {
private:
  UndirectedGraph& graph;
public:
  GeometryBuilder(UndirectedGraph& graph) :
    graph(graph) {}
  virtual ~GeometryBuilder() {}

  void build();
};

#endif /* GEOMETRYBUILDER_H_ */
