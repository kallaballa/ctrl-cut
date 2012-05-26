#ifndef TRAVERSE_H_
#define TRAVERSE_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <set>

#include "cut/geom/Geometry.hpp"
#include "cut/graph/SegmentGraph.hpp"

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
/*
template<typename TsegmentInputIterator>
void check(TsegmentInputIterator first, TsegmentInputIterator last) {
  BOOST_CONCEPT_ASSERT((SegmentInputIterator<TsegmentInputIterator>));

  std::set<Segment> uniq_segments;

  for (TsegmentInputIterator it_s = first; it_s != last; ++it_s) {
    const Segment& seg = *it_s;
    // assert the segments are globally unique
    assert(uniq_segments.find(seg) == uniq_segments.end() && uniq_segments.find(Segment(seg.second, seg.first)) == uniq_segments.end());
    uniq_segments.insert(seg);
  }
}
*/
void dump(std::ostream& os, Route::iterator first, Route::iterator last) {
  os << "<route>" << std::endl;

  for(Route::iterator it = first; it != last; ++it)
    os << *it;

  os << "</route>" << std::endl;
}

void dump(const std::string &filename, Route::iterator first, Route::iterator last) {
  std::ofstream os(filename.c_str(), std::ios_base::out);
  dump(os, first,last);
  os.close();
}


/*
struct join_strings_visitor: public planar_face_traversal_visitor {
  SegmentGraph& graph;
  Route& strings;

  join_strings_visitor(SegmentGraph& graph, Route& strings) :
    graph(graph), strings(strings) {
  }

  void begin_face() {}
  void end_face() {}
  void next_edge(SegmentGraph::Edge e) {
    Segment& seg = graph[e];
    if(strings.append(seg))
      strings.push_back(seg);
  }
}; */

/*void make_linestrings(Route& strings, SegmentList::const_iterator first, SegmentList::const_iterator  last, SegmentGraph& segGraph) {
  LOG_INFO_STR("make linestrings");
  LOG_DEBUG_MSG("strings before", strings.size());
  create_planar_graph(segGraph, first, last);
  join_strings_visitor vis = *new join_strings_visitor(segGraph, strings);
  traverse_planar_faces(segGraph , vis);
  LOG_DEBUG_MSG("strings after", strings.size());

#ifdef DEBUG
  check_linestrings(strings.beginStrings(), strings.endStrings());
#endif
}

void make_linestrings(Route& strings, SegmentList::const_iterator first, SegmentList::const_iterator last) {
  SegmentGraph segGraph;
  make_linestrings(strings, first, last, segGraph);
}
*/
/*
void make_linestrings(Route& strings, SegmentList::const_iterator first, SegmentList::const_iterator  last, SegmentGraph& segGraph) {
  LOG_INFO_STR("make linestrings");
  LOG_DEBUG_MSG("strings before", strings.size());
  create_planar_graph(segGraph, first, last);
  join_strings_visitor vis = *new join_strings_visitor(segGraph, strings);
  traverse_planar_faces(segGraph , vis);
  LOG_DEBUG_MSG("strings after", strings.size());

#ifdef DEBUG
  check_linestrings(strings.beginStrings(), strings.endStrings());
#endif
}
*/
template<typename Graph>
bool build_planar_embedding(typename Graph::Embedding& embedding, Graph& graph) {
 // Test for planarity and compute the planar embedding as a side-effect
 if (boyer_myrvold_planarity_test(boyer_myrvold_params::graph = graph,
     boyer_myrvold_params::embedding = &embedding[0]))
   return true;
 else
   return false;
}


template<typename Visitor>
inline void traverse_planar_faces(SegmentGraph& graph, Visitor& visitor) {
  SegmentGraph::Embedding Embedding;
/*
  std::map<SegmentGraph::Edge, graph_traits<SegmentGraph>::edges_size_type > e_index_map;
  graph_traits<SegmentGraph>::edges_size_type edge_count = 0;
  graph_traits<SegmentGraph>::edge_iterator ei, ei_end;

  for(tie(ei, ei_end) = edges(graph); ei != ei_end; ++ei)
    e_index_map[*ei] = edge_count++;
*/
//  SegmentGraph::Embedding embedding(num_vertices(graph));
 // assert(build_planar_embedding(embedding, graph));
  //planar_face_traversal(graph, &embedding[0], visitor,make_assoc_property_map(e_index_map));
}



#endif /* TRAVERSE_H_ */
