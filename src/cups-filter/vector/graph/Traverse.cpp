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
#include "util/LaserConfig.h"
#include <boost/graph/properties.hpp>
#include <boost/graph/metric_tsp_approx.hpp>

struct join_strings_visitor: public planar_face_traversal_visitor {
  SegmentGraph& graph;
  StringList& strings;
  SegmentString* current;

  join_strings_visitor(SegmentGraph& graph, StringList& strings) :
    graph(graph), strings(strings), current(NULL) {
  }

  void begin_face() {
//    std::cerr << "begin face " << std::endl;
  }

  void end_face() {
  }

  void next_edge(SegmentGraph::Edge e) {
    const Segment* seg = graph[e].segment;

    if (graph[e].owner == NULL) {
      if (current == NULL || !current->addSegment(*seg)) {
        current = new SegmentString();
        current->addSegment(*seg);
        strings.push_back(current);
      }

      graph[e].owner = current;
    }
  }
};

void check_linestrings(StringList::iterator first, StringList::iterator last) {
  std::set<Segment> segments;

  for(StringList::iterator it = first; it != last; ++it) {
    const SegmentString& string = **it;

    for(SegmentString::SegmentConstIter it_s =  string.beginSegments(); it_s != string.endSegments(); ++it_s) {
      const Segment& seg = **it_s;
      // assert the segments are globally unique
      assert(segments.find(seg) == segments.end() && segments.find(Segment(seg.second, seg.first, seg.settings)) == segments.end());
      // assert coordinate clipping to laser bed
      assert(seg.first.x >= 0 && seg.first.x < LaserConfig::inst().device_width
          && seg.first.y >= 0 && seg.first.y < LaserConfig::inst().device_height
          && seg.second.x >= 0 && seg.second.x < LaserConfig::inst().device_width
          && seg.second.y >= 0 && seg.second.y < LaserConfig::inst().device_height
      );
      segments.insert(seg);
    }
  }
}

void dump_linestrings(const std::string &filename, StringList::iterator first, StringList::iterator last) {
  std::ofstream os(filename.c_str(), std::ios_base::out);
  dump_linestrings(os, first,last);
  os.close();
}

void dump_linestrings(std::ostream& os, StringList::iterator first, StringList::iterator last) {
  os << "<cut>" << std::endl;
  for(StringList::iterator it = first; it != last; ++it)
    os << **it;
  os << "</cut>" << std::endl;
}

void make_linestrings(StringList& strings, SegmentList::iterator first, SegmentList::iterator  last, SegmentGraph& segGraph) {
  LOG_INFO_STR("make linestrings");
  LOG_DEBUG_MSG("strings before", strings.size());
  create_planar_graph(segGraph, first, last);
  join_strings_visitor vis = *new join_strings_visitor(segGraph, strings);
  traverse_planar_faces(segGraph, vis);
  LOG_DEBUG_MSG("strings after", strings.size());
#ifdef DEBUG
  check_linestrings(strings.begin(), strings.end());
#endif
}

void make_linestrings(StringList& strings, SegmentList::iterator first, SegmentList::iterator last) {
  SegmentGraph segGraph;
  make_linestrings(strings, first, last, segGraph);
}

void travel_linestrings(StringList& strings, StringList::iterator first, StringList::iterator  last) {
  LOG_INFO_STR("travel linestrings");
  LOG_DEBUG_MSG("strings before", strings.size());

  StringGraph graph;
  StringGraph::Vertex v_origin = create_complete_graph_from_point(graph, * new Point(0,0),first, last);

  typedef boost::property_map<boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, TieProperty, StringProperty>, double StringProperty::*>::type WeightMap;
  WeightMap weight_map(get(&StringProperty::weight, graph));
  std::map<StringGraph::Vertex, graph_traits<StringGraph>::vertices_size_type > v_index_map;
  graph_traits<StringGraph>::vertices_size_type vertex_count = 0;
  graph_traits<StringGraph>::vertex_iterator vi, vi_end;

  for(tie(vi, vi_end) = vertices(graph); vi != vi_end; ++vi)
    v_index_map[*vi] = vertex_count++;

  vector<StringGraph::Vertex> route;
  double len = 0.0;
  boost::metric_tsp_approx_from_vertex(graph, v_origin, weight_map, boost::make_tsp_tour_len_visitor(graph, std::back_inserter(route), len, weight_map));

  const StringGraph::Vertex* nextVertex = NULL;
  const SegmentString* nextString = NULL;
  std::set<const SegmentString*> traversedStrings;

  for (vector<StringGraph::Vertex>::iterator it = route.begin(); it
      != route.end(); ++it) {
    nextVertex = &(*it);
    nextString = graph[*it].owner;

    if (nextString != NULL && traversedStrings.find(nextString) == traversedStrings.end()) {
      strings.push_back(nextString);
      traversedStrings.insert(nextString);
    }
  }

  LOG_DEBUG_MSG("strings after", strings.size());
  LOG_INFO_MSG("Tour length", len);
#ifdef DEBUG
  check_linestrings(strings.begin(), strings.end());
#endif
}


