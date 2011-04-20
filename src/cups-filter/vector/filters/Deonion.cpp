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
#include <assert.h>
#include "util/Logger.h"
#include "vector/Edge.h"
#include "vector/Vertex.h"
#include "vector/Polyline.h"
#include "vector/Cut.h"
#include "Deonion.h"

Deonion::~Deonion() {
}

void walkTheEdge(Polyline* p, Polyline* skin, Edge* current, bool cw)
{
  Edge *next_edge = NULL;

  float edge_slope = current->getSlope(true);

  float slope_diff;
  float min_slope_diff = 2 * M_PI;

  /*
   * transfer the edge from the polyline to the skin unless it was
   * already consumed which should only happen during backtracking
   */
  if (p->contains(current)) {
    p->remove(current);
    skin->append(current);
  } else {
    assert(!cw);
  }

  /*
   * from the edges that are connected to the end vertex of the current
   * edge search the one with the least clock wise angle
   */
  for (Vertex::iterator it = current->end()->begin(); it != current->end()->end(); it++) {
    Edge *candidate = *it;

    /*
     * can't consider a candidate that is identical to the current
     * edge or that has already been consumed
     */
    if (candidate == current || !p->contains(candidate))
      continue;

    // make sure edges point into search direction
    if (candidate->start() != current->end())
      candidate->invertDirection();

    /*
     * don't consider candidates that are coincidental unless it's the
     * last one to prevent going in circles.
     */
    if(p->size() > 1 && candidate->end() == current->start())
      continue;

    float candidate_slope = candidate->getSlope();

    slope_diff = edge_slope - candidate_slope;

    //normalize radians to make them comparable
    if (slope_diff < 0) {
      slope_diff += 2*M_PI;
    }

    //save the edge with the least clockwise angle (= least slop_diff)
    if (slope_diff < min_slope_diff) {
      min_slope_diff = slope_diff;
      next_edge = candidate;
    }
  }

  if (next_edge == NULL && cw) {
    // if we traversed a closed polyline, emit a skin and continue,
    // else, we reached a possible blind alley and must backtrack
    if (skin->isClosed()) return;

    // invert search direction (=counter-clockwise)
    current->invertDirection();
    walkTheEdge(p, skin, current, !cw);
  } else if (next_edge != NULL) {
    if (!cw) {
      cw = true;
    }
    walkTheEdge(p, skin, next_edge, cw);
  }

  //polyline fully consumed
}

void Deonion::filter(Cut *cut)
{
  LOG_INFO_MSG("Deonion", cut->size());

  Polyline* p;
  Cut::PolylineVector skins;
  size_t psize;
  for (Cut::iterator it_c = cut->begin(); it_c != cut->end(); it_c++) {
    p = *it_c;

    // recurse until the polyline is consumed
    while ((psize = p->size()) > 0) {
      Polyline *skin = new Polyline();
      /*
       * traverse the polyline from the most left vertex and tranfer
       * edges until the polyline is fully consumed or the skin forms
       * a closed polyline
       */
      walkTheEdge(p, skin, p->findLeftmostClockwise(), true);

      //make sure polylines are consumed to prevent endless loops;
      assert(skin->size() > 0 &&  p->size() < psize);
      skins.push_back(skin);
    }
  }
  cut->swap(skins);
}
