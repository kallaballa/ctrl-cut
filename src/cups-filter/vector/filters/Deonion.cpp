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
#include "util/Logger.h"
#include "vector/Edge.h"
#include "vector/Vertex.h"
#include "vector/Polyline.h"
#include "vector/Cut.h"
#include "Deonion.h"

Deonion::~Deonion() {
}

void walkTheEdge(Polyline* p, Polyline* skin, Edge* edge, bool cw)
{
  Edge *next_edge = NULL;

  float edge_slope = edge->getSlope(true);

  float slope_diff;
  float min_slope_diff = 2 * M_PI;

  if (p->contains(edge)) {
    p->remove(edge);
    skin->append(edge);
  }

  for (Vertex::iterator it = edge->end()->begin(); it != edge->end()->end(); it++) {
    Edge *candidate = *it;

    if (candidate == edge || !p->contains(candidate))
      continue;

    if (candidate->start() != edge->end())
      candidate->invertDirection();

    if(p->size() > 1 && candidate->end() == edge->start())
      continue;

    float candidate_slope = candidate->getSlope();

    slope_diff = edge_slope - candidate_slope;
    if (slope_diff < 0) {
      slope_diff += 2*M_PI;
    }

    if (slope_diff < min_slope_diff) {
      min_slope_diff = slope_diff;
      next_edge = candidate;
    }
  }

  if (next_edge == NULL && cw) {
    // if we traversed a closed polyline, emit a skin and continue,
    // else, we reached a possible blind alley and must backtrack
    if (skin->isClosed()) return;

    edge->invertDirection();
    walkTheEdge(p, skin, edge, !cw);
  } else if (next_edge != NULL) {
    if (!cw) {
      cw = true;
    }
    walkTheEdge(p, skin, next_edge, cw);
  }
}

void Deonion::filter(Cut *cut)
{
  LOG_INFO_MSG("Deonion", cut->size());

  Polyline* p;
  Cut::PolylineVector skins;

  for (Cut::iterator it_c = cut->begin(); it_c != cut->end(); it_c++) {
    p = *it_c;

    while (p->size() > 0) {
      Polyline *skin = new Polyline();
      walkTheEdge(p, skin, p->findLeftmostClockwise(), true);
      skins.push_back(skin);
    }

  }
  cut->swap(skins);
}
