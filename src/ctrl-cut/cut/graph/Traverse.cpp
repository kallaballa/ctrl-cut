/*
 * Ctrl-Cut - A laser cutter CUPS driver
 * Copyright (C) 2009-2010 Amir Hassan <amir@viel-zu.org> and Marius Kintel <marius@kintel.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <set>
#include "CutGraph.h"
#include "Traverse.h"
#include "util/Logger.h"
#include <boost/graph/properties.hpp>
#include <boost/graph/metric_tsp_approx.hpp>

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
      strings.create(seg);
  }
};

void check_linestrings(Route::StringIter first, Route::StringIter last) {
  std::set<Segment> segments;

  for(Route::StringIter it = first; it != last; ++it) {
    const SegmentString& string = *it;

    for(SegmentString::const_iterator it_s =  string.begin(); it_s != string.end(); ++it_s) {
      const Segment& seg = *it_s;
      // assert the segments are globally unique
      assert(segments.find(seg) == segments.end() && segments.find(Segment(seg.second, seg.first, seg)) == segments.end());
      segments.insert(seg);
    }
  }
}

void dump_linestrings(const std::string &filename, Route::StringIter first, Route::StringIter last) {
  std::ofstream os(filename.c_str(), std::ios_base::out);
  dump_linestrings(os, first,last);
  os.close();
}

void dump_linestrings(std::ostream& os, Route::StringIter first, Route::StringIter last) {
  os << "<cut>" << std::endl;
  for(Route::StringIter it = first; it != last; ++it)
    os << *it;
  os << "</cut>" << std::endl;
}

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

void make_linestrings(Route& strings, SegmentList::const_iterator first, SegmentList::const_iterator last) {
  SegmentGraph segGraph;
  make_linestrings(strings, first, last, segGraph);
}

#include "boost/function_output_iterator.hpp"
#include "boost/bind.hpp"

void travel_linestrings(Route& route, Route::StringIter first, Route::StringIter last) {
  using namespace boost;
  LOG_INFO_STR("travel linestrings");
  LOG_DEBUG_MSG("strings before", route.size());

  StringGraph graph;
  StringGraph::Vertex v_origin = create_complete_graph_from_point(graph, Point(0,0),first, last);

  typedef boost::property_map<boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, Point, StringProperty>, double StringProperty::*>::type WeightMap;
  WeightMap weight_map(get(&StringProperty::weight, graph));

  vector<StringGraph::Vertex> tour;
  double len = 0.0;
  RouteBuilder<StringGraph> rb(graph, route);
  boost::metric_tsp_approx_from_vertex(graph, v_origin, weight_map, boost::make_tsp_tour_visitor(make_function_output_iterator(boost::bind<void>(rb, _1))));

  LOG_DEBUG_MSG("strings after", route.size());
  LOG_INFO_MSG("Tour length", len);

#ifdef DEBUG
  check_linestrings(route.beginStrings(), route.endStrings());
#endif
}


