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
  LOG_DEBUG_MSG("String count before TSP: ",model.numStrings());
  // if the tsp should mind the origin too
  bool mindOrigin = true;

  CutGraph& graph = * new CutGraph();
  CutGraph::Vertex v_origin;

  if(mindOrigin) {
    v_origin = CutGraph::createCompleteGraphFromPoint(graph, * new Point(0,0),model.beginStrings(), model.endStrings());
  } else {
    CutGraph::createCompleteGraph(graph, model.beginStrings(), model.endStrings());
  }

  typedef boost::property_map<CutGraph, boost::edge_weight_t>::type WeightMap;
  WeightMap weight_map(get(boost::edge_weight, graph));

  vector<CutGraph::Vertex> route;
  double len = 0.0;
  if(mindOrigin)
    boost::metric_tsp_approx_from_vertex(graph, v_origin, weight_map, boost::make_tsp_tour_len_visitor(graph, std::back_inserter(route), len, weight_map));
  else
    boost::metric_tsp_approx(graph, boost::make_tsp_tour_len_visitor(graph, std::back_inserter(route), len, weight_map));

  const CutGraph::Vertex* lastVertex = NULL;
  const CutGraph::Vertex* nextVertex = NULL;
  const SegmentString* lastString = NULL;
  const SegmentString* nextString = NULL;
  for (vector<CutGraph::Vertex>::iterator it = route.begin(); it
      != route.end(); ++it) {
    nextVertex = &(*it);
    if (lastVertex != NULL) {
      nextString = graph.getSegmentString(*it);
      if (nextString != NULL && nextString != lastString) {
        model.remove(*nextString);
        model.add(*nextString);
        lastString = nextString;
      }
    }
    lastVertex = nextVertex;
  }

  LOG_INFO_MSG("Tour length: ", len);
  LOG_DEBUG_MSG("String count after TSP: ",model.numStrings());
}
