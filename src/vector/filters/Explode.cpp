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

void Explode::filter(Cut *cut) {
  LOG_INFO_STR("Explode");

  Vertex *intersec = NULL;
  Edge *ls1, *ls2;

  LstEdge::iterator it_i;
  LstEdge::iterator it_j;

  int cntLines = 0;
  for (it_i = cut->freeEdges.begin(); it_i != cut->freeEdges.end(); it_i++) {
    cntLines++;

    for (it_j = cut->freeEdges.begin(); it_j != cut->freeEdges.end(); it_j++) {
      ls2 = *it_j;
      ls1 = *it_i;

      if (it_i == cut->freeEdges.end())
        break;

      if (ls1 == ls2)
        continue;

      if ((intersec = ls1->intersects(ls2)) != NULL) {
        intersec = cut->mapVertex(intersec);

        if (!ls1->getStart()->equals(intersec) && !ls1->getEnd()->equals(
                                                                         intersec)) {
          it_i = cut->removeEdge(it_i, true);
          cut->createEdge((Vertex*) ls1->getStart(), intersec, ls1->getPower());
          cut->createEdge((Vertex*) ls1->getEnd(), intersec, ls1->getPower());
        }

        if (!ls2->getStart()->equals(intersec) && !ls2->getEnd()->equals(
                                                                         intersec)) {
          it_j = cut->removeEdge(it_j, true);
          cut->createEdge((Vertex*) ls2->getStart(), intersec, ls2->getPower());
          cut->createEdge((Vertex*) ls2->getEnd(), intersec, ls2->getPower());
        }
      }
    }
  }
}
