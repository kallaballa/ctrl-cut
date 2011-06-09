#ifndef GEOMETRYBUILDER_H_
#define GEOMETRYBUILDER_H_

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

struct linestrings_visitor: public planar_face_traversal_visitor {
  CutGraph& graph;
  SegmentString* current;
  vector<SegmentString*>& strings;
  property_map<CutGraph, edge_string_t>::type const& e_string;

  linestrings_visitor(CutGraph& graph, vector<SegmentString*>& strings) :
    graph(graph), current(NULL), strings(strings),  e_string(get(edge_string, graph)) {
  }

  void begin_face() {
//    std::cerr << "begin face " << std::endl;
  }

  void end_face() {
    SegmentString::iterator it;
    if (current != NULL) {
      for (it = current->begin(); it != current->end(); ++it) {
     //   std::cout << *it << " ";
      }
      strings.push_back(current);
    }
  //  std::cout << std::endl << "end face " << std::endl;
    current = NULL;
  }


  void next_edge(CutGraph::Edge e) {
    const Segment* seg = get_segment(e, graph);

    if (e_string[e] == NULL) {
      if (current == NULL) {
        current = new SegmentString();
        current->push_back(seg);
      } else {
        const Point& last = current->back()->second;
        bool front = false;
        if (last == seg->first) {
          current->push_back(seg);
        } else if (last == seg->second) {
          current->push_back(new Segment(seg->second, seg->first, seg->settings));
        } else {
          front = true;
          const Point& first = current->front()->first;
          if (first == seg->first) {
            current->push_front(new Segment(seg->second, seg->first, seg->settings));
          } else if (first == seg->second) {
            current->push_front(seg);
          } else {
            assert(false);
          }
        }
      }

      put(e_string, e, current);
    }
  }
};

std::vector<SegmentString*>& traverse_segmentstrings(CutGraph& graph) {
  // Test for planarity and compute the planar embedding as a side-effect
  typedef std::vector< CutGraph::Edge > vec_t;
  std::vector<vec_t> embedding(num_vertices(graph));
  std::vector<SegmentString*>& strings = (* new std::vector<SegmentString*>());

  if (boyer_myrvold_planarity_test(boyer_myrvold_params::graph = graph,
      boyer_myrvold_params::embedding = &embedding[0]))
    std::cerr<< "Input graph is planar" << std::endl;
  else
    std::cerr << "Input graph is not planar" << std::endl;

  linestrings_visitor vis(graph, strings);
  planar_face_traversal(graph, &embedding[0], vis);
  return strings;
}

#endif /* GEOMETRYBUILDER_H_ */
