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
#include "Vertex.h"
#include "raster/DownSample.h"
#include "stddef.h"
#include "climits"
#include "iostream"
#include <cmath>
#include <assert.h>
#include <algorithm>

int Polyline::cnt = 0;

Polyline::Polyline() {
  this->id++;
}

bool Polyline::isClosed() const
{
  return this->edges.size() > 0 && (*this->edges.front())[0] == (*this->edges.back())[1];
}

void Polyline::prepend(Edge *edge) {
  edge->join(this);
  this->edges.insert(this->edges.begin(),edge);
  this->bb.invalidate();
}

void Polyline::append(Edge *edge) {
  edge->join(this);
  this->edges.push_back(edge);
  this->bb.invalidate();
}

void Polyline::remove(Edge *edge) {
  Polyline::iterator it = find(this->edges.begin(), this->edges.end(), edge);
  if (it != this->edges.end()) {
    (*it)->leave(this);
    this->edges.erase(it);
    this->bb.invalidate();
  }
}

bool Polyline::contains(Edge *edge) {
  Polyline::iterator it = find(this->edges.begin(), this->edges.end(), edge);
  return it != this->edges.end();
}


BBox *Polyline::getBoundingBox()
{
  if (!this->bb.isValid()) {
    for (Polyline::iterator it = this->begin(); it != this->end(); it++) {
      Edge &e = **it;
      this->bb.adjustTo(e[0][0], e[0][1]);
      this->bb.adjustTo(e[1][0], e[1][1]);
    }
  }
  return &this->bb;
}

void Polyline::reverseOrder() {
  if(this->size() == 1)
    this->front()->invertDirection(); // if there is only one edge  reversing the container has no effect, therefore invert the edge
  else
    reverse(this->begin(), this->end()); // end is closer to the last reference location, therefore reverse the polyline
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
    startx = (**it)[0][0];
    endx = (**it)[1][0];

    if (startx < min_x) {
      min_x = startx;
      leftmostvertex = (*it)->start();
    }
    if (endx < min_x) {
      min_x = endx;
      leftmostvertex = (*it)->end();
    }
  }

  // Find next clockwise edge
  Edge *found = NULL;
  float steapest = 2 * M_PI;
  for (Vertex::iterator it_c = leftmostvertex->begin(); it_c != leftmostvertex->end(); it_c++) {
    Edge *edge = *it_c;

    // make sure we're pointing into the positive halfsphere
    if ((*edge)[0][0] > (*edge)[1][0]) {
      edge->invertDirection();
    }

    float slope = edge->getSlope();
    if (slope < steapest) {
      steapest = slope;
      found = edge;
    }
  }

  assert(found->getSlope() >= 0);

  return found;
}

ostream& operator <<(ostream &os, Polyline &pl) {
  os << "<polyline id=\"" << pl.id << "\" >" << std::endl;
  BBox* bb = pl.getBoundingBox();
  os << "<bbox distToOrigin=\"" << bb->distanceToOrigin() << "\" ul.x=\""
      << bb->ul[0] << "\" ul.y=\"" << bb->ul[1] << "\" lr.x=\"" << bb->lr[0]
      << "\" lr.y=\"" << bb->lr[1] << "\" />" << std::endl;
  for (Polyline::iterator it = pl.edges.begin(); it != pl.edges.end(); it++) {
    os << *((Edge*) *it);
  }
  os << "</polyline>" << std::endl;
  return os;
}
