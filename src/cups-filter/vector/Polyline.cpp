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
#include "Polyline.h"
#include "Edge.h"
#include <assert.h>

int Polyline::cnt = 0;

Polyline::Polyline() {
  this->id++;
}

void Polyline::add(Edge* ls) {
  this->push_back(ls);
  this->bb.invalidate();
}

void Polyline::remove(Edge* ls) {
  Polyline::iterator it = find(ls);
  if (it != (Polyline::iterator) NULL) {
    this->erase(it);
    this->bb.invalidate();
  }
}

bool Polyline::contains(Edge* ls) {
  Polyline::iterator it = find(ls);
  return it != (Polyline::iterator) NULL && it != end();
}


BBox* Polyline::getBoundingBox() {
  if (!this->bb.isValid()) {
    Edge* e;
    Vertex* start;
    Vertex* end;

    for (Polyline::iterator it = this->begin(); it != this->end(); it++) {
      e = (*it);
      start = e->start;
      end = e->end;
      this->bb.adjustTo(start->getX(), start->getY());
      this->bb.adjustTo(end->getX(), end->getY());
    }
  }
  return &this->bb;
}

/*!
 Finds the "first leftmost clockwise" edge. This is the edge connected to
 the leftmost vertex which is the first we would traverse when doing a clockwise
 traversal. This is the edge which has the smallest positive angle to the positive Y axis.
 */
Edge* Polyline::findLeftmostClockwise() {
  int min_x = INT_MAX;
  Vertex *leftmostvertex = NULL;
  int startx;
  int endx;

  // Find leftmost vertex
  for (Polyline::iterator it = this->begin(); it != this->end(); it++) {
    startx = (*it)->start->getX();
    endx = (*it)->end->getX();

    if (startx < min_x) {
      min_x = startx;
      leftmostvertex = (*it)->start;
    }
    if (endx < min_x) {
      min_x = endx;
      leftmostvertex = (*it)->end;
    }
  }

  // Find next clockwise edge
  SetEdge connected = leftmostvertex->getAttachedEdges();

  Edge *edge = NULL;
  float steapest = 2 * M_PI;
  Edge *ls;
  for (SetEdge::iterator it_c = connected.begin(); it_c != connected.end(); it_c++) {
    ls = *it_c;

    // make sure we're pointing into the positive halfsphere
    if (ls->start->getX() > ls->end->getX()) {
      ls->invertDirection();
    }

    float slope = ls->getSlope();
    if (slope < steapest) {
      steapest = slope;
      edge = ls;
    }
  }

  assert(edge->getSlope() >= 0);

  return edge;
}

ostream& operator <<(ostream &os, Polyline &pl) {
  os << "<polyline id=\"" << pl.id << "\" >" << std::endl;
  BBox* bb = pl.getBoundingBox();
  os << "<bbox distToOrigin=\"" << bb->distanceToOrigin() << "\" ul_x=\""
      << bb->ul_x << "\" ul_y=\"" << bb->ul_y << "\" lr_x=\"" << bb->lr_x
      << "\" lr_y=\"" << bb->lr_y << "\" \\>" << std::endl;
  for (Polyline::iterator it = pl.begin(); it != pl.end(); it++) {
    os << *((Edge*) *it);
  }
  os << "</polyline>" << std::endl;
  return os;
}
