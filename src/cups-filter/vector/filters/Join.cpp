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
#include "vector/Polyline.h"
#include "vector/Cut.h"

#include <boost/format.hpp>

using boost::format;

Join::~Join() {
  // TODO Auto-generated destructor stub
}

/**
 * Recursive function
 */
void find_connected(set<Edge*> *occupied, Polyline *polyline,
                    Edge* current) {
  Edge* candidate;

  occupied->insert(current);

  for (Vertex::iterator it = current->end->begin(); it != current->end->end(); it++) {
    candidate = *it;
    if (candidate == current || occupied->find(candidate) != occupied->end())
      continue;
    if (candidate->start != current->end) {
      candidate->end = candidate->start;
      candidate->start = current->end;
    }
    polyline->add(candidate);
    find_connected(occupied, polyline, candidate);
  }
}

/*
 * Iterate over all free edges and collect connected ones into polylines.
 */
void Join::filter(Cut *cut)
{
  LOG_INFO_STR("Join");

  set<Edge*> *occupied = new set<Edge*>();

  Edge* ls;
  list<Edge*>::iterator it;
  int cnt = 0;
  Mesh mesh = cut->getMesh();
  for (it = mesh.begin(); it != mesh.end(); it++) {
    ls = *it;

    Polyline *polyline = new Polyline();

    if (occupied->find(ls) == occupied->end()) {
      polyline->add(ls);
      find_connected(occupied, polyline, ls);
      cut->add(polyline);
    }
    cnt++;
  }

  for(Cut::iterator it = cut->begin(); it != cut->end(); it++) {
    Polyline *p = *it;
    for(Polyline::iterator it_e = p->begin(); it_e != p->end(); it_e++) {
      mesh.remove(*it_e);
    }
  }
}
