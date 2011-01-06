/*
 * EpilogCUPS - A laser cutter CUPS driver
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

using namespace std;

int Polyline::cnt = 0;

Polyline::Polyline() { this->id++; }

void Polyline::add(Edge* ls) {
  edges.push_back(ls);
}

VecEdge::iterator Polyline::find(Edge* ls) {
  for (VecEdge::iterator it = this->edges.begin(); it != this->edges.end(); it++) {
    if (*it == ls)
      return it;
  }

  return (VecEdge::iterator)NULL;
}

bool Polyline::contains(Edge* ls) {
  VecEdge::iterator it = find(ls);
  return  it != (VecEdge::iterator) NULL && it != edges.end();
}

void Polyline::remove(Edge* ls) {
  VecEdge::iterator it = find(ls);
  if(it != (VecEdge::iterator) NULL)
    edges.erase(it);
}

int Polyline::count() {
  return edges.size();
}

/*!
  Finds the "first leftmost clockwise" edge. This is the edge connected to
  the leftmost vertex which is the first we would traverse when doing a clockwise
  traversal. This is the edge which has the smallest positive angle to the positive Y axis.
*/
Edge *Polyline::findLeftmostClockwise() 
{
  int min_x = INT_MAX;
  Vertex *leftmostvertex = NULL;
  int startx;
  int endx;

  // Find leftmost vertex
  for (VecEdge::iterator it = edges.begin(); it != edges.end(); it++) {
    startx = (*it)->getStart()->getX();
    endx = (*it)->getEnd()->getX();

    if (startx < min_x) {
      min_x = startx;
      leftmostvertex = (*it)->getStart();
    }
    if (endx < min_x) {
      min_x = endx;
      leftmostvertex = (*it)->getEnd();
    }
  }

  cerr << "leftmostvertex: " << *leftmostvertex;

  // Find next clockwise edge
  SetEdge connected = leftmostvertex->getAttachedEdges();

  Edge *edge = NULL;
  float steapest = 2*M_PI;
  Edge *ls;
  for (SetEdge::iterator it_c = connected.begin(); it_c != connected.end(); it_c++) {
    ls = *it_c;

    // make sure we're pointing into the positive halfsphere
    if (ls->getStart()->getX() > ls->getEnd()->getX()) {
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
  os << "<polyline id=\"" << pl.id << "\" >"
     << std::endl;

  for (VecEdge::iterator it = pl.edges.begin(); it
         != pl.edges.end(); it++) {
    os << *((Edge*) *it);
  }
  os << "</polyline>" << std::endl;
  return os;
}
