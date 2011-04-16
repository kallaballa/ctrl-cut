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
#include "StreamLine.h"

StreamLine::~StreamLine() {}


void StreamLine::filter(Cut *cut)
{
  LOG_INFO_MSG("StreamLine", cut->size());

  Polyline *current = NULL;
  Polyline *previous = NULL;

  Vertex *p_end = NULL;
  Vertex *c_start = NULL;
  Vertex *c_end = NULL;

  int s_dist;
  int e_dist;

  for (Cut::iterator it_c = cut->begin(); it_c != cut->end(); it_c++) {
    current = *it_c;

    c_start = current->front()->start();
    c_end = current->back()->end();

    if(previous != NULL) {
      p_end = previous->back()->end();
      s_dist = p_end->distance(*c_start);
      e_dist = p_end->distance(*c_end);
    } else {
      Vertex at_origin(0,0);
      s_dist = at_origin.distance(*c_start);
      e_dist = at_origin.distance(*c_end);
    }

    if(e_dist < s_dist)
      current->reverseEdgeOrder();

    previous = current;
  }
}
