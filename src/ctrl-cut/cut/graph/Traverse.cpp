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
};

void dump(const std::string &filename, Route::iterator first, Route::iterator last) {
  std::ofstream os(filename.c_str(), std::ios_base::out);
  dump(os, first,last);
  os.close();
}

void dump(std::ostream& os, Route::iterator first, Route::iterator last) {
  os << "<cut>" << std::endl;

  for(Route::iterator it = first; it != last; ++it)
    os << *it;

  os << "</cut>" << std::endl;
}

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
