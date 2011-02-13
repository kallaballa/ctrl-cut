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
#include "Edge.h"

// initialize counter
int Edge::cnt = 0;

void Edge::invertDirection() {
  Vertex* tmp = this->start;
  this->start = end;
  this->end = tmp;
}

void Edge::detach() {
  this->start->detach(this);
  this->end->detach(this);
}

/*!
  Returns angle to the positive Y axis
 */
float Edge::getSlope(bool invert) {
  int d_x;
  int d_y;
  if (invert) {
    d_x = this->start->getX() - this->end->getX();
    d_y = this->start->getY() - this->end->getY();
  } else {
    d_x = this->end->getX() - this->start->getX();
    d_y = this->end->getY() - this->start->getY();
  }

  // Swap x and y since we're measuring relative to the Y axis.
  // We also negate the Y axis since positive Y points downwards (left handed)
  return atan2(d_x, -d_y);
}

Vertex* Edge::intersects(Edge *otherLine) {
  float denom = ((otherLine->end->getY() - otherLine->start->getY())
                 * (this->end->getX() - this->start->getX()))
    - ((otherLine->end->getX() - otherLine->start->getX())
       * (this->end->getY() - this->start->getY()));

  float nume_a = ((otherLine->end->getX() - otherLine->start->getX())
                  * (this->start->getY() - otherLine->start->getY()))
    - ((otherLine->end->getY() - otherLine->start->getY())
       * (this->start->getX() - otherLine->start->getX()));

  float nume_b = ((this->end->getX() - this->start->getX())
                  * (this->start->getY() - otherLine->start->getY()))
    - ((this->end->getY() - this->start->getY()) * (this->start->getX()
                                                    - otherLine->start->getX()));

  if (denom == 0.0f) {
    if (nume_a == 0.0f && nume_b == 0.0f) {
      return NULL; //COINCIDENCE
    }
    return NULL; //PARALLEL;
  }

  float ua = nume_a / denom;
  float ub = nume_b / denom;

  if (ua >= 0.0f && ua <= 1.0f && ub >= 0.0f && ub <= 1.0f) {
    // Get the intersection LSPoint.
    int ix = this->start->getX() + ua * (this->end->getX()
                                         - this->start->getX());
    int iy = this->start->getY() + ua * (this->end->getY()
                                         - this->start->getY());

    Vertex* intersection = new Vertex(ix, iy);
    if ((this->start->equals(intersection) || this->end->equals(
                                                                intersection)) && (otherLine->start->equals(intersection)
                                                                                   || otherLine->end->equals(intersection)))
      return NULL; //tip intersection
    else
      return intersection;

  }

  return NULL; //NOT_INTERSECTING;
}

ostream& operator <<(ostream &os, Edge &e) {
  os << "<edge power=\"" << e.power << "\" id=\"" << e.id << "\" >" << std::endl;
  os << *e.start;
  os << *e.end;
  os << "</edge>" << std::endl;
  return os;
}
