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
  SegmentGraph& graph = SegmentGraph::createSegmentGraph(model.beginSegments(), model.endSegments());

  typedef boost::property_map<SegmentGraph, boost::edge_weight_t>::type WeightMap;
  WeightMap weight_map(get(boost::edge_weight, graph));

  vector<SegmentGraph::Vertex> route;
  SegmentGraph::Vertex v_origin =*graph.findVertex(* new Segment(* new Point(), * new Point(), * new CutSettings(-1,-1,-1)));
  double len = 0.0;
  boost::metric_tsp_approx_from_vertex(graph, v_origin, weight_map, boost::make_tsp_tour_len_visitor(graph, std::back_inserter(route), len, weight_map));
  const Segment* nextSegment = NULL;
  SegmentString* string = NULL;
  int edgecount = 0;
  for (vector<SegmentGraph::Vertex>::iterator it = route.begin(); it
      != route.end(); ++it) {
    if (*it == v_origin)
      continue;
    nextSegment = graph.getSegment(*it);
    edgecount++;
    if (nextSegment != NULL) {
      if (string == NULL || !string->addSegment(*nextSegment)) {
        string = new SegmentString();
        string->addSegment(*nextSegment);
        model.add(*string);
      }
    }
  }

  LOG_INFO(edgecount);
  LOG_INFO_MSG("After: ",model.numStrings());
}
