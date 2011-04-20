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

bool sortEdgeByX(Edge* e1, Edge* e2) {
  uint32_t e1_minX = min((*e1)[0][0], (*e1)[1][0]);
  uint32_t e2_minX = min((*e2)[0][0], (*e2)[1][0]);
  return e1_minX < e2_minX;
}

/*
 * Split edges at intersection points.
 */
void Explode::filter(Cut *cut)
{
  LOG_INFO_STR("Explode");

  Vertex *intersec = NULL;
  Edge *pick, *candidate;

  Mesh &mesh = cut->getMesh();
  mesh.edges.sort(sortEdgeByX);
  list<Edge*> stripe;
  list<Edge*> carryOver;
  Edge* first = mesh.front();
  uint32_t stripeStep = 200;
  uint32_t stripeLimit = min((*first)[0][0], (*first)[1][0]);
  list<Edge*>::iterator it_m = mesh.begin();

  while(it_m != mesh.end()) {
    stripe = carryOver;
    carryOver.clear();
    stripeLimit+=stripeStep;

    for (; it_m != mesh.end(); it_m++) {
      pick = *it_m;

      uint32_t s_x = (*pick)[0][0];
      uint32_t e_x = (*pick)[1][0];

      if(s_x > stripeLimit && e_x > stripeLimit)
        break;

      if(s_x > stripeLimit || e_x > stripeLimit)
        carryOver.push_back(pick);

      stripe.push_back(pick);
    }

    for (list<Edge*>::iterator it_i = stripe.begin(); it_i != stripe.end(); it_i++) {
      for (list<Edge*>::iterator it_j = it_i; ++it_j != stripe.end(); ) {
        pick = *it_i;
        candidate = *it_j;

        // check if pick does intersect candidate
        if ((intersec = pick->intersects(*candidate)) != NULL) {

          // FIXME: We should inherit speed and frequency too

          // if pick doesnt tip intersect remove it and split it in two
          if (!((*pick)[0] == *intersec) && !((*pick)[1] == *intersec)) {
            pick->detach();
            mesh.remove(pick);

            mesh.create(pick->start(), intersec, pick->power);
            mesh.create(pick->end(), intersec, pick->power);
          }

          // if candidate doesnt tip intersect remove it and split it in two
          if (!((*candidate)[0] == *intersec) && !((*candidate)[1] == *intersec)) {
            candidate->detach();
            mesh.remove(candidate);

            mesh.create(candidate->start(), intersec, candidate->power);
            mesh.create(candidate->end(), intersec, candidate->power);
          }
        }
      }
    }
  }
}
