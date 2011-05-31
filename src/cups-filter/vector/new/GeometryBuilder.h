#ifndef GEOMETRYBUILDER_H_
#define GEOMETRYBUILDER_H_

#include <iostream>
#include <vector>

#include <boost/graph/planar_face_traversal.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>

#include "Geometry.h"

using std::vector;
using boost::planar_face_traversal;
using boost::boyer_myrvold_planarity_test;
using boost::graph_traits;

using namespace boost;

template<typename Graph>
struct output_visitor: public planar_face_traversal_visitor {
  Graph& graph;
  PolyLine* current;
  typename property_map<Graph, edge_geom_t>::type const& e_geom;

  output_visitor(Graph& graph) :
    graph(graph), current(NULL), e_geom(get(edge_geom, graph)) {
  }

  void begin_face() {
    std::cout << "begin face " << std::endl;
  }
  void end_face() {
    PolyLine::iterator it;
    if (current != NULL) {
      for (it = current->begin(); it != current->end(); ++it) {
        std::cout << *it << " ";
      }
    }
    std::cout << std::endl << "end face " << std::endl;
    current = NULL;
  }

  template<typename Edge>
  void next_edge(Edge e) {
    const ConstSegment& seg = segment(e, graph);

    if (e_geom[e] == NULL) {
      if (current == NULL) {
        current = new Linestring();
        current->push_back(seg.first);
        current->push_back(seg.second);
      } else {
        Point& last = current->back();
        bool front = false;
        if (equals(last, seg.first)) {
          current->push_back(seg.second);
        } else if (equals(last, seg.second)) {
          current->push_back(seg.first);
        } else {
          front = true;
          Point& first = current->front();
          if (equals(first, seg.first)) {
            current->push_front(seg.second);
          } else if (equals(first, seg.second)) {
            current->push_front(seg.first);
          } else {
            assert(false);
          }
        }
      }

      put(e_geom, e, current);
    }
  }
};

template<typename Graph, typename View>
void build(Graph& graph, View& view) {
  typedef typename graph_traits<Graph>::edge_descriptor Edge;
  typedef std::vector<Edge> vec_t;

  // Test for planarity and compute the planar embedding as a side-effect
  std::vector<vec_t> embedding(num_vertices(graph));
  if (boyer_myrvold_planarity_test(boyer_myrvold_params::graph = graph,
      boyer_myrvold_params::embedding = &embedding[0]))
    std::cout << "Input graph is planar" << std::endl;
  else
    std::cout << "Input graph is not planar" << std::endl;

  output_visitor<Graph> vis(graph);
  planar_face_traversal(view, &embedding[0], vis);
}

#endif /* GEOMETRYBUILDER_H_ */
