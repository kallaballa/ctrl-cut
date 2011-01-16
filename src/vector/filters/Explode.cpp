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

#include "../../util/Logger.h"
#include "Explode.h"
#include "../VTypes.h"
#include "../Edge.h"
#include "../Cut.h"

using boost::format;

Explode::~Explode() {
}

/*
 * Split edges at intersection points.
 */
void Explode::filter(Cut *cut) {
  LOG_INFO_STR("Explode");

  Vertex *intersec = NULL;
  Edge *pick, *candidate;

  LstEdge::iterator it_i;
  LstEdge::iterator it_j;

  int cntLines = 0;
  for (it_i = cut->freeEdges.begin(); it_i != cut->freeEdges.end(); it_i++) {
    cntLines++;

    for (it_j = cut->freeEdges.begin(); it_j != cut->freeEdges.end(); it_j++) {
      pick = *it_i;
      candidate = *it_j;

      //no more free edges
      if (it_i == cut->freeEdges.end())
        break;

      // collision
      if (pick == candidate)
        continue;

      // check if pick does intersect candidate
      if ((intersec = pick->intersects(candidate)) != NULL) {
        // register the resulting vertex
        intersec = cut->mapVertex(intersec);


        // if pick doesnt tip intersect remove it and split it in two
        if (!pick->getStart()->equals(intersec) && !pick->getEnd()->equals(intersec)) {
          it_i = cut->removeEdge(it_i, true);
          cut->createEdge((Vertex*) pick->getStart(), intersec, pick->getPower());
          cut->createEdge((Vertex*) pick->getEnd(), intersec, pick->getPower());
        }

        // if candidate doesnt tip intersect remove it and split it in two
        if (!candidate->getStart()->equals(intersec) && !candidate->getEnd()->equals(
                                                                         intersec)) {
          it_j = cut->removeEdge(it_j, true);
          cut->createEdge((Vertex*) candidate->getStart(), intersec, candidate->getPower());
          cut->createEdge((Vertex*) candidate->getEnd(), intersec, candidate->getPower());
        }
      }
    }
  }
}
