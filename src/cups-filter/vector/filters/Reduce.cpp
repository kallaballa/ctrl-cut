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
#include "vector/Edge.h"
#include "vector/Vertex.h"
#include "vector/Polyline.h"
#include "vector/Cut.h"

/*!
 * Reduce number of vertices in polylines by approximating the polylines with fewer line segments.
 */
void Reduce::filter(Cut *cut)
{
  LOG_INFO_STR("Reduce");

  // FIXME: Only follow edges with the same settings

  Cut::PolylineVector polylines;

  // Reduce each polyline separately
  for (Cut::iterator it = cut->begin(); it != cut->end(); it++) {
    Polyline *pl = *it;
    Polyline *newpl = new Polyline();
    polylines.push_back(newpl);

    // Select a start iterator
    Polyline::iterator startit = pl->begin();
    // Detach vertices since we're rebuilding the polylines
    (*startit)->detach();

    // Walk the entire polyline
    Polyline::iterator pit;
    for (pit = startit; ++pit != pl->end(); ) {
      // Detach vertices since we're rebuilding the polylines
      (*pit)->detach();

      // Span an edge to the current vertex for testing
      Edge consider((*startit)->start(), (*pit)->end());
      float largest = 0;
      Polyline::iterator largestit;
      
      // Check distance from every intermediate vertex
      for (Polyline::iterator pit2 = startit; pit2 != pit; pit2++) {
        float d = consider.distance((**pit2)[1]);
        if (d > largest) {
          largest = d;
          largestit = pit2;
        }
      }

      // We exceeded the epsilon, split the edge and continue
      if (largest > this->epsilon) {
        Edge *newe = new Edge((*startit)->start(), (*largestit)->end(),
                              (*startit)->power, (*startit)->speed, (*startit)->frequency);
        newe->attach();
        newpl->add(newe);
        startit = largestit + 1;
      }
    }
    // Add last line
    Edge *newe = new Edge((*startit)->start(), pl->back()->end(),
                          (*startit)->power, (*startit)->speed, (*startit)->frequency);
    newe->attach();
    newpl->add(newe);
  }

  cut->swap(polylines);
  //  cut->rebuildMesh();
}
