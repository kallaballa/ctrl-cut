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
#include "Reduce.h"
#include "util/Logger.h"
#include "cut/graph/Traverse.h"
#include "cut/model/CutModel.h"

bool isCrossing(SegmentGraph& graph, Point&  p) {
  SegmentGraph::Vertex v;
  typedef typename boost::graph_traits<SegmentGraph>::edge_descriptor Edge;

  if(graph.findVertex(v,p)) {
    boost::graph_traits<SegmentGraph>::out_edge_iterator oe_it, oe_end;
    boost::tie(oe_it,oe_end) = boost::out_edges(v, graph);
    if(std::distance(oe_it,oe_end) > 2)
      return true;
  }
  return false;
}

/*!
 * Reduce number of vertices in polylines by approximating the polylines with fewer line segments.
 */
void reduce(Route &route, Route::iterator begin, Route::iterator end) {
  // FIxME reduce seems to cause memory corruption when called twice on a model
  if(1)
    return;

  LOG_INFO_STR("Reduce");
  LOG_DEBUG_MSG("Strings before", route.pointView().size());
  //LineString simplified;
  for(Route::iterator it = begin; it != end; ++it) {
//    boost::geometry::simplify(*it, simplified, 0.5);
  }

  LOG_DEBUG_MSG("Strings after",  route.pointView().size());
}
