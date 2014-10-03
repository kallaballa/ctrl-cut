#include "Planar.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <set>

#include "cut/geom/Geometry.hpp"
#include "cut/geom/algorithms/Algorithms.hpp"
#include "cut/graph/SegmentGraph.hpp"
#include "cut/graph/Traverse.hpp"

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

join_strings_visitor::join_strings_visitor(const SegmentGraph& graph, Route& sink) :
  graph(&graph), sink(&sink) {
}

void join_strings_visitor::begin_face() {}
void join_strings_visitor::end_face() {}
void join_strings_visitor::next_edge(SegmentGraph::Edge e) {
  if(index.insert(e).second) {
    const SegmentProperty* seg = &(*graph)[e];
    add(*sink, *static_cast<const Segment*>(seg));
  }
}

void make_planar_faces(const Route& src, Route& sink) {
  LOG_INFO_STR("make planar");
  SegmentGraph segGraph;
  for(const SegmentPtr seg : segments(src)) {
    segGraph.addSegment(*seg.get());
  }

  join_strings_visitor vis(segGraph, sink);
  traverse_planar_faces(segGraph , vis);
}
