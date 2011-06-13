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
#include <math.h>
#include <iostream>
#include <limits>
#include "util/Logger.h"
#include "vector/Edge.h"
#include "vector/Vertex.h"
#include "vector/Polyline.h"
#include "vector/Cut.h"
#include "StreamLine.h"

StreamLine::~StreamLine() {}

void StreamLine::filter(Cut *cut)
{
  LOG_INFO_MSG("StreamLine", cut->size());

  Polyline *current = NULL;
  Polyline *previous = NULL;

  Vertex *ref_vertex = NULL;
  Vertex *c_start = NULL;
  Vertex *c_end = NULL;

  int s_dist;
  int e_dist;
  Vertex at_origin(0,0);

  for (Cut::iterator it_c = cut->begin(); it_c != cut->end(); it_c++) {
    current = *it_c;

    if(previous != NULL)
      ref_vertex = previous->back()->end();
    else
      ref_vertex = &at_origin;

    if(current->isClosed()) {
      int minDist = std::numeric_limits<int>::max();
      Polyline::iterator* minDistIt = NULL;

      for (Polyline::iterator it_p = current->begin(); it_p != current->end(); it_p++) {
        Edge *e = *it_p;

        s_dist = ref_vertex->distance(*e->start());
        e_dist = ref_vertex->distance(*e->end());

        int dist = std::min(s_dist, e_dist);

        if (dist < minDist) {
          minDistIt = &it_p;
          minDist = dist;
        }
      }
      swap_ranges(*minDistIt, current->end(), current->begin());
    } else {
      c_start = current->front()->start();
      c_end = current->back()->end();


      s_dist = ref_vertex->distance(*c_start);
      e_dist = ref_vertex->distance(*c_end);

      if(e_dist < s_dist)
        current->reverseOrder();
    }
    previous = current;
  }
}
