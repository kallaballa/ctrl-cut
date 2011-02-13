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

#include "util/Logger.h"
#include "Explode.h"
#include "vector/VTypes.h"
#include "vector/Edge.h"
#include "vector/Cut.h"

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

  Mesh::iterator it_i;
  Mesh::iterator it_j;

  int cntLines = 0;
  Mesh& mesh = cut->getMesh();
  for (it_i = mesh.begin(); it_i != mesh.end(); it_i++) {
    cntLines++;

    for (it_j = mesh.begin(); it_j != mesh.end(); it_j++) {
      pick = *it_i;
      candidate = *it_j;

      //no more free edges
      if (it_i == mesh.end())
        break;

      // collision
      if (pick == candidate)
        continue;

      // check if pick does intersect candidate
      if ((intersec = pick->intersects(candidate)) != NULL) {

        // if pick doesnt tip intersect remove it and split it in two
        if (!pick->getStart()->equals(intersec) && !pick->getEnd()->equals(intersec)) {
          it_i = mesh.removeEdge(it_i, true);
          mesh.createEdge((Vertex*) pick->getStart(), intersec, pick->getPower());
          mesh.createEdge((Vertex*) pick->getEnd(), intersec, pick->getPower());
        }

        // if candidate doesnt tip intersect remove it and split it in two
        if (!candidate->getStart()->equals(intersec) && !candidate->getEnd()->equals(
                                                                         intersec)) {
          it_j = mesh.removeEdge(it_j, true);
          mesh.createEdge((Vertex*) candidate->getStart(), intersec, candidate->getPower());
          mesh.createEdge((Vertex*) candidate->getEnd(), intersec, candidate->getPower());
        }
      }
    }
  }
}
