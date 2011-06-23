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

/*!
 * Reduce number of vertices in polylines by approximating the polylines with fewer line segments.
 */
void Reduce::filter(CutModel& model)
{
  LOG_INFO_STR("Reduce");
/*
  // Reduce each polyline separately
  for (CutModel::StringIter it = model.beginStrings(); it != model.endStrings(); it++) {
    const SegmentString *pl = *it;
    SegmentString *newpl = new SegmentString();

    // Select a start iterator
    CutModel::SegmentConstIter startit = pl->beginSegments();

    // Walk the entire string
    CutModel::SegmentConstIter pit;
    for (pit = startit; ++pit != pl->endSegments(); ) {
      float largest = 0;
      CutModel::SegmentConstIter largestit;
      if ((*startit)->first != (*pit)->second) {
        // Span a segment to the current vertex for testing
        Segment consider((*startit)->first, (*pit)->second, (*startit)->settings);

        // Check distance from every intermediate vertex
        for (CutModel::SegmentConstIter pit2 = startit; pit2 != pit; pit2++) {
          float d = consider.distance((**pit2)[1]);
          if (d > largest) {
            largest = d;
            largestit = pit2;
          }
        }
      }
      else {
        // If we span a closed polygon (start == end), we still check whether we should collapse
        // it to a line. FIXME: This might not be desirable in the end.

        // Check distance from every intermediate vertex to this vertex
        for (CutModel::SegmentConstIter pit2 = startit; pit2 != pit; pit2++) {
          float d = (*startit)->first.distance((*pit2)->second);
          if (d > largest) {
            largest = d;
            largestit = pit2;
          }
        }
      }

      // We exceeded the epsilon, split the edge and continue
      if (largest > this->epsilon) {
        Segment *newe = new Segment((*startit)->first, (*largestit)->second, (*startit)->settings);
        newpl->addSegment(*newe);
        startit = largestit++;
      }
    }
    // Add last line
    Segment *newe = new Segment((*startit)->first, pl->backSegments()->second, (*startit)->settings);
    newpl->addSegment(*newe);
    model.replaceString(it,*newpl);
  }

  //  model.rebuildMesh();*/
}
