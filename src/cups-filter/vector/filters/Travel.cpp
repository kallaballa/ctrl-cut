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

#include "Travel.h"
#include "util/Logger.h"
#include <boost/graph/properties.hpp>
#include <boost/graph/metric_tsp_approx.hpp>

void Travel::filter(CutModel& model) {
  LOG_INFO_STR("Travel");
  LOG_INFO_MSG("Before: ",model.numStrings());
  CutGraph& graph = CutGraph::createCutGraph(model.beginStrings(), model.endStrings());

  typedef boost::graph_traits<CutGraph>::vertex_iterator VertexIterator;
  VertexIterator it_i;
  VertexIterator itend_i;
  VertexIterator it_j;
  VertexIterator itend_j;
  CutGraph::Vertex vi;
  CutGraph::Vertex vj;
  // add origin and make it the source point of the tsp traversal later
  CutGraph::Vertex v_origin = graph.addVertex(* new Point(0,0));
  int added_edges = 0;
  // create a complete graph by adding all possible movements (edges) between the end points of segment strings
  for(boost::tie(it_i,itend_i) = boost::vertices(graph); it_i != itend_i; ++it_i) {
    vi = *it_i;
    for(boost::tie(it_j,itend_j) = boost::vertices(graph); it_j != itend_j; ++it_j) {
      vj = *it_j;

      // identical vertices or edge already exists -> continue
      if(vi == vj  || boost::edge(vi, vj, graph).second)
        continue;

      const Point& p_vi = get_point(vi, graph);
      const Point& p_vj = get_point(vj, graph);
      double weight = p_vi.distance(p_vj);

      GeomProperty geomProp(0, StringProperty(0, IndexProperty(graph.edge_count++, WeightProperty(weight))));
      add_edge(vi, vj, geomProp, graph);
      added_edges++;
    }
  }
  LOG_INFO(added_edges);
  typedef boost::property_map<CutGraph, boost::edge_weight_t>::type WeightMap;
  WeightMap weight_map(get(boost::edge_weight, graph));

  vector<CutGraph::Vertex> route;
  double len = 0.0;
  boost::metric_tsp_approx_from_vertex(graph, v_origin, weight_map, boost::make_tsp_tour_len_visitor(graph, std::back_inserter(route), len, weight_map));
  CutGraph::Vertex* lastVertex = NULL;
  const SegmentString* currentString = NULL;
  const SegmentString* nextString = NULL;

  for(vector<CutGraph::Vertex>::iterator it = route.begin(); it != route.end(); ++it) {
    CutGraph::Vertex* nextVertex = &(*it);

    if(lastVertex != NULL && nextVertex != NULL) {
      const std::pair<CutGraph::Edge, bool>& edge_result = boost::edge(*lastVertex, *nextVertex, graph);
      nextString = get_segment_string(edge_result.first, graph);

      if(nextString != NULL && currentString != nextString) {
        model.remove(*nextString);
        model.add(*nextString);
        currentString = nextString;
      }
    }

    lastVertex = nextVertex;
  }

  LOG_INFO_MSG("After: ",model.numStrings());
}
