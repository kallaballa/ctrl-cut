#ifndef PLANAR_H_
#define PLANAR_H_

#include <set>
#include "cut/geom/Geometry.hpp"
#include "cut/graph/SegmentGraph.hpp"
#include "cut/graph/Traverse.hpp"
#include <boost/graph/planar_face_traversal.hpp>

struct join_strings_visitor: public boost::planar_face_traversal_visitor {
  const SegmentGraph* graph;
  Route* sink;
  std::set<SegmentGraph::Edge> index;

  join_strings_visitor(const SegmentGraph& graph, Route& sink);
  void begin_face();
  void end_face();
  void next_edge(SegmentGraph::Edge e);
};

void make_planar_faces(const Route& src, Route& sink);

#endif /* PLANAR_H */
