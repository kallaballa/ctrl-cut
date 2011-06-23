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

#include "CutGraph.h"
#include "Traverse.h"
#include "util/Logger.h"
#include <boost/graph/properties.hpp>
#include <boost/graph/metric_tsp_approx.hpp>

struct join_strings_visitor: public planar_face_traversal_visitor {
  CutGraph& graph;
  StringList& strings;
  SegmentString* current;

  join_strings_visitor(CutGraph& graph, StringList& strings) :
    graph(graph), strings(strings), current(NULL) {
  }

  void begin_face() {
//    std::cerr << "begin face " << std::endl;
  }

  void end_face() {
    if (current != NULL)
      strings.push_back(current);

    current = NULL;
  }

  void next_edge(CutGraph::Edge e) {
    const Segment* seg = graph.getSegment(e);

    if (graph.getOwner(e) == NULL) {
      if (current == NULL || !current->addSegment(*seg)) {
        current = new SegmentString();
        current->addSegment(*seg);
      }

      graph.setOwner(e, *current);
    }
  }
};


void make_linestrings(StringList& strings, SegmentList::iterator first, SegmentList::iterator  last) {
  LOG_INFO_STR("make linestrings");
  LOG_DEBUG_MSG("strings before", strings.size());
  CutGraph graph;
  create_segment_graph(graph, first, last);
  join_strings_visitor vis = *new join_strings_visitor(graph, strings);
  traverse_planar_faces(graph, vis);
  LOG_DEBUG_MSG("strings after", strings.size());
}

void travel_linestrings(StringList& strings, StringList::iterator first, StringList::iterator  last) {
  LOG_INFO_STR("travel linestrings");
  LOG_DEBUG_MSG("strings before", strings.size());

  CutGraph graph;
  CutGraph::Vertex v_origin = create_complete_graph_from_point(graph, * new Point(0,0),first, last);

  typedef boost::property_map<boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, VertexGeometry, EdgeGeometry>, double EdgeGeometry::*>::type WeightMap;
  WeightMap weight_map(get(&EdgeGeometry::weight, graph));
  std::map<CutGraph::Vertex, graph_traits<CutGraph>::vertices_size_type > v_index_map;
  graph_traits<CutGraph>::vertices_size_type vertex_count = 0;
  graph_traits<CutGraph>::vertex_iterator vi, vi_end;

  for(tie(vi, vi_end) = vertices(graph); vi != vi_end; ++vi)
    v_index_map[*vi] = vertex_count++;

  vector<CutGraph::Vertex> route;
  double len = 0.0;
  boost::metric_tsp_approx_from_vertex(graph, v_origin, weight_map, boost::make_tsp_tour_len_visitor(graph, std::back_inserter(route), len, weight_map));

  const CutGraph::Vertex* lastVertex = NULL;
  const CutGraph::Vertex* nextVertex = NULL;
  const SegmentString* lastString = NULL;
  const SegmentString* nextString = NULL;
  for (vector<CutGraph::Vertex>::iterator it = route.begin(); it
      != route.end(); ++it) {
    nextVertex = &(*it);
    if (lastVertex != NULL) {
      nextString = graph.getOwner(*it);
      if (nextString != NULL && nextString != lastString) {

        /*FIXME enable rotating of entry points for closed strings optionally
        const Point& first = *graph.getPoint(*it);
        if(nextString->isClosed()) {
          if(*nextString->frontPoints() != first) {
            SegmentString* rotated = new SegmentString(*nextString);
            rotated->rotate(first);
            strings.push_back(rotated);
          }
        } else */
        strings.push_back(nextString);

        lastString = nextString;
      }
    }
    lastVertex = nextVertex;
  }

  LOG_DEBUG_MSG("strings after", strings.size());
  LOG_INFO_MSG("Tour length", len);
}

// Test for planarity and compute the planar embedding as a side-effect
bool build_planar_embedding(CutGraph::Embedding& embedding, CutGraph& graph) {
 // Test for planarity and compute the planar embedding as a side-effect
 if (boyer_myrvold_planarity_test(boyer_myrvold_params::graph = graph,
     boyer_myrvold_params::embedding = &embedding[0]))
   return true;
 else
   return false;
}
