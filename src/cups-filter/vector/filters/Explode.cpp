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
#include "vector/Edge.h"
#include "vector/Vertex.h"
#include "vector/Cut.h"
#include "Explode.h"

/*
 * Split edges at intersection points.
 */
void Explode::filter(Cut *cut)
{
  LOG_INFO_STR("Explode");

  Vertex *intersec = NULL;
  Edge *pick, *candidate;

  Mesh &mesh = cut->getMesh();
  for (Mesh::iterator it_i = mesh.begin(); it_i != mesh.end(); it_i++) {
    for (Mesh::iterator it_j = it_i; ++it_j != mesh.end(); ) {
      pick = *it_i;
      candidate = *it_j;

      // check if pick does intersect candidate
      if ((intersec = pick->intersects(*candidate)) != NULL) {

        // FIXME: We should inherit speed and frequency too

        // if pick doesnt tip intersect remove it and split it in two
        if (!(*pick)[0].equals(intersec) && !(*pick)[1].equals(intersec)) {
          it_i = mesh.eliminate(it_i);
          mesh.create(pick->start(), intersec, pick->power);
          mesh.create(pick->end(), intersec, pick->power);
        }

        // if candidate doesnt tip intersect remove it and split it in two
        if (!(*candidate)[0].equals(intersec) && !(*candidate)[1].equals(intersec)) {
          it_j = mesh.eliminate(it_j);
          mesh.create(candidate->start(), intersec, candidate->power);
          mesh.create(candidate->end(), intersec, candidate->power);
        }
      }
    }
  }
}
