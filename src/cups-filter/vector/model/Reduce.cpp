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
#include "vector/graph/Traverse.h"

bool isShared(SegmentGraph& graph, const Point& p) {
  SegmentGraph::Vertex* v;
  if((v = graph.findVertex(p)) != NULL) {
    boost::graph_traits<SegmentGraph>::out_edge_iterator oe_it, oe_end;
    const SegmentString* last_owner = NULL;
    for(boost::tie(oe_it,oe_end) = boost::out_edges(*v, graph); oe_it != oe_end; ++oe_it) {
      if(last_owner != NULL && last_owner != graph[*oe_it].owner) {
        return true;
      } else
        last_owner = graph[*oe_it].owner;
    }
  }
  return false;
}

/*!
 * Reduce number of vertices in polylines by approximating the polylines with fewer line segments.
 */
void reduce_linestrings(CutModel &model, float epsilon)
{
  if(epsilon == 0.0f)
    return;

  LOG_INFO_STR("Reduce");
  LOG_DEBUG_MSG("Segments before", model.size());
  SegmentGraph graph;
  StringList join;
  make_linestrings(join, model.begin(), model.end(), graph);
  CutModel newModel;

  // Reduce each polyline separately
  for (StringList::iterator it = join.begin(); it != join.end(); ++it) {
    const SegmentString *string = *it;
    SegmentString *newstring = new SegmentString();

    // Select a start iterator
    SegmentString::SegmentConstIter startit = string->beginSegments();

    // Walk the entire string
    SegmentString::SegmentConstIter  pit;
    for (pit = startit; ++pit != string->endSegments(); ) {
      const Segment& startSegment = **startit;
      float largest = 0;
      SegmentString::SegmentConstIter  largestit;
      if (!string->isClosed()) {
        // Span a segment to the current vertex for testing
        Segment consider(startSegment.first, (*pit)->second, startSegment.settings);

        // Check distance from every intermediate vertex
        for (SegmentString::SegmentConstIter  pit2 = startit; pit2 != pit; pit2++) {
          float d = consider.distance((**pit2)[1]);
          if (d > largest) {
            largest = d;
            largestit = pit2;
          }

          if(isShared(graph,(**pit2)[1])) {
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
        for (SegmentString::SegmentConstIter  pit2 = startit; pit2 != pit; pit2++) {
          float d = startSegment.first.distance((*pit2)->second);
          if (d > largest) {
            largest = d;
            largestit = pit2;
          }

          if(isShared(graph,(**pit2)[1])) {
            largest = epsilon + 1;
            largestit = pit2;
            break;
          }
        }
      }

      // We exceeded the epsilon, split the edge and continue
      if (largest > epsilon) {
        Segment *newseg = new Segment(startSegment.first, (*largestit)->second, startSegment.settings);
        newstring->addSegment(*newseg);
        startit = ++largestit;
      }
    }
    // Add last line
    Segment *newe = new Segment((*startit)->first, string->backSegments()->second, (*startit)->settings);
    newstring->addSegment(*newe);

    for(SegmentString::SegmentConstIter it_s = newstring->beginSegments(); it_s != newstring->endSegments(); ++it_s)
      newModel.add(**it_s);
  }

  model = newModel;
  LOG_DEBUG_MSG("Segments after", model.size());
}
