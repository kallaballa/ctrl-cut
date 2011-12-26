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

bool isShared(SegmentGraph& graph, Point&  p) {
  SegmentGraph::Vertex v;
  if(graph.findVertex(v,p)) {
    boost::graph_traits<SegmentGraph>::out_edge_iterator oe_it, oe_end;
    CutModel* last_owner = NULL;
    for(boost::tie(oe_it,oe_end) = boost::out_edges(v, graph); oe_it != oe_end; ++oe_it) {
      Segment& current = graph[*oe_it];
      if(last_owner != NULL && current.hasParent() && last_owner != &current.getParent())
        return true;
      else
        last_owner = &current.getParent();
    }
  }
  return false;
}

/*!
 * Reduce number of vertices in polylines by approximating the polylines with fewer line segments.
 */
void reduce_linestrings(CutModel &model, float epsilon)
{
  // FIxME reduce seems to cause memory corruption when called twice on a model
  epsilon = 0.0f;

  if(epsilon == 0.0f)
    return;

  LOG_INFO_STR("Reduce");
  LOG_DEBUG_MSG("Segments before", model.size());
  SegmentGraph graph;
  Route join(model);
  make_linestrings(join, model.begin(), model.end(), graph);
  CutModel newModel(model);

  // Reduce each polyline separately
  for (Route::StringIter it = join.beginStrings(); it != join.endStrings(); ++it) {
    SegmentString& string = *it;
    // Select a start iterator
    SegmentString::iterator startit = string.begin();

    // Walk the entire string
    SegmentString::iterator  pit;
    for (pit = startit; ++pit != string.end(); ) {
      const Segment& startSegment = *startit;
      float largest = 0;
      SegmentString::iterator largestit;
      if (!string.isClosed()) {
        // Span a segment to the current vertex for testing
        Segment consider(startSegment.first, (*pit).second, startSegment);

        // Check distance from every intermediate vertex
        for (SegmentString::iterator pit2 = startit; pit2 != pit; pit2++) {
          float d = consider.distance((*pit2)[1]);
          if (d > largest) {
            largest = d;
            largestit = pit2;
          }

          if(isShared(graph,(*pit2)[1])) {
            largest = epsilon + 1;
            largestit = pit2;
            break;
          }
        }
      }
      else {
        // If we span a closed polygon (start == end), we still check whether we should collapse
        // it to a line. FIXME: This might not be desirable in the end.

        // Check distance from every intermediate vertex to this vertex
        for (SegmentString::iterator pit2 = startit; pit2 != pit; pit2++) {
          float d = startSegment.first.distance((*pit2).second);
          if (d > largest) {
            largest = d;
            largestit = pit2;
          }

          if(isShared(graph,(*pit2)[1])) {
            largest = epsilon + 1;
            largestit = pit2;
            break;
          }
        }
      }

      // We exceeded the epsilon, split the edge and continue
      if (largest > epsilon) {
        newModel.create(startSegment.first, (*largestit).second, startSegment);
        startit = ++largestit;
      }
    }
    // Add last line
    newModel.create((*startit).first, string.back().second, *startit);
  }

  model = newModel;
  LOG_DEBUG_MSG("Segments after", model.size());
}
