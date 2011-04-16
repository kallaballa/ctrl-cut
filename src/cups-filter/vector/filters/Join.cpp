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
#include "Join.h"
#include "vector/Edge.h"
#include "vector/Vertex.h"
#include "vector/Polyline.h"
#include "vector/Cut.h"

/**
 * Recursive function
 */
static void find_connected(Polyline *polyline, Edge *current, bool forward) {
  current->join(polyline);
  Vertex* v;

  if(forward)
    v = current->end();
  else
    v = current->start();

  for (Vertex::iterator it = v->begin(); it != v->end(); it++) {
    Edge *candidate = *it;
    if (candidate == current || candidate->isPolylineMember())
      continue;

    if (forward) {
      if (candidate->start() != current->end())
        candidate->invertDirection();

      polyline->append(candidate);
    } else {
      if (candidate->end() != current->start())
        candidate->invertDirection();

      polyline->prepend(candidate);
    }
    find_connected(polyline, candidate, forward);
  }
}

/*
 * Iterate over all free edges and collect connected ones into polylines.
 */
void Join::filter(Cut *cut)
{
  LOG_INFO_STR("Join");

  Mesh &mesh = cut->getMesh();
  for (Mesh::iterator it = mesh.begin(); it != mesh.end(); it++) {
    Edge *edge = *it;

    if (!edge->isPolylineMember()) {
      Polyline *polyline = new Polyline();
      polyline->append(edge);
      find_connected(polyline, edge, true);
      find_connected(polyline, edge, false);
      cut->add(polyline);
    }
  }

  for (Cut::iterator it = cut->begin(); it != cut->end(); it++) {
    Polyline *p = *it;
    for (Polyline::iterator it_e = p->begin(); it_e != p->end(); it_e++) {
      mesh.remove(*it_e);
    }
  }
}
