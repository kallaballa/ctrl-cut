/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#include "Planar.hpp"

#include "cut/geom/Geometry.hpp"
#include "cut/geom/algorithms/Algorithms.hpp"
#include "cut/graph/SegmentGraph.hpp"
#include "cut/graph/Traverse.hpp"
#include <boost/graph/planar_face_traversal.hpp>


using std::vector;
using namespace boost;

struct join_strings_visitor: public boost::planar_face_traversal_visitor {
  const SegmentGraph* graph;
  Route* sink;
  std::set<SegmentGraph::Edge> index;

  join_strings_visitor(const SegmentGraph& graph, Route& sink) :
    graph(&graph), sink(&sink) {
  }

  void begin_face() {}
  void end_face() {}
  void next_edge(SegmentGraph::Edge e) {
    if(index.insert(e).second) {
      const SegmentProperty* seg = &(*graph)[e];
      add(*sink, *static_cast<const Segment*>(seg));
    }
  }
};

void make_planar_faces(const Route& src, Route& sink) {
  LOG_INFO_STR("make planar faces");
  SegmentGraph segGraph;
  for(const SegmentPtr seg : segments(src)) {
    segGraph.addSegment(*seg.get());
  }

  join_strings_visitor vis(segGraph, sink);
  traverse_planar_faces(segGraph , vis);
}
