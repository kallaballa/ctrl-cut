#ifndef PLANAR_H_
#define PLANAR_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <set>

#include "cut/geom/Geometry.hpp"
#include "cut/graph/SegmentGraph.hpp"
#include "cut/graph/Traverse.hpp"
#include "cut/graph/sink/CreateSink.hpp"

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

template<typename TpointRangeOutputIterator>
struct join_strings_visitor: public planar_face_traversal_visitor {
  const SegmentGraph* graph;
  TpointRangeOutputIterator sink;

  join_strings_visitor(const SegmentGraph& graph, TpointRangeOutputIterator& sink) :
    graph(&graph), sink(sink) {
  }

  void begin_face() {}
  void end_face() {}
  void next_edge(SegmentGraph::Edge e) {
    const SegmentProperty* seg = &(*graph)[e];
    *sink++ = *static_cast<const Segment*>(seg);
  }
};

template<
  typename TpointRange
  >
void makePlanar(TpointRange& pointRange, TpointRange& sink) {
  LOG_INFO_STR("make planar");
  AddSink<TpointRange> addSink(sink);
  SegmentGraph segGraph;
  createGeometryGraph(segmentView(pointRange), segGraph);
  join_strings_visitor<AddSink<TpointRange> > vis(segGraph, addSink);
  traverse_planar_faces(segGraph , vis);
}


#endif /* PLANAR_H */
