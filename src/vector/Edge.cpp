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
#include "Edge.h"

using namespace std;
int Edge::cnt = 0;

Edge::Edge(Vertex *start, Vertex *end, int power) {
  this->id = cnt++;
  this->v[0] = start;
  this->v[1] = end;
  this->power = power;
}

void Edge::setPower(int power) {
  this->power = power;
}

int Edge::getPower() {
  return this->power;
}

void Edge::invertDirection() {
  Vertex* tmp = getStart();
  setStart(getEnd());
  setEnd(tmp);
}

void Edge::detach() {
  this->getStart()->detach(this);
  this->getEnd()->detach(this);
}

/*!
  Returns angle to the positive Y axis
 */
float Edge::getSlope(bool invert) {
  int d_x;
  int d_y;
  if (invert) {
    d_x = this->v[0]->getX() - this->v[1]->getX();
    d_y = this->v[0]->getY() - this->v[1]->getY();
  } else {
    d_x = this->v[1]->getX() - this->v[0]->getX();
    d_y = this->v[1]->getY() - this->v[0]->getY();
  }

  // Swap x and y since we're measuring relative to the Y axis.
  // We also negate the Y axis since positive Y points downwards (left handed)
  return atan2(d_x, -d_y);
}

Vertex* Edge::intersects(Edge *otherLine) {
  float denom = ((otherLine->v[1]->getY() - otherLine->v[0]->getY())
                 * (this->v[1]->getX() - this->v[0]->getX()))
    - ((otherLine->v[1]->getX() - otherLine->v[0]->getX())
       * (this->v[1]->getY() - this->v[0]->getY()));

  float nume_a = ((otherLine->v[1]->getX() - otherLine->v[0]->getX())
                  * (this->v[0]->getY() - otherLine->v[0]->getY()))
    - ((otherLine->v[1]->getY() - otherLine->v[0]->getY())
       * (this->v[0]->getX() - otherLine->v[0]->getX()));

  float nume_b = ((this->v[1]->getX() - this->v[0]->getX())
                  * (this->v[0]->getY() - otherLine->v[0]->getY()))
    - ((this->v[1]->getY() - this->v[0]->getY()) * (this->v[0]->getX()
                                                    - otherLine->v[0]->getX()));

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
    int ix = this->v[0]->getX() + ua * (this->v[1]->getX()
                                         - this->v[0]->getX());
    int iy = this->v[0]->getY() + ua * (this->v[1]->getY()
                                         - this->v[0]->getY());

    Vertex* intersection = new Vertex(ix, iy);
    if ((this->v[0]->equals(intersection) || this->v[1]->equals(
                                                                intersection)) && (otherLine->v[0]->equals(intersection)
                                                                                   || otherLine->v[1]->equals(intersection)))
      return NULL; //tip intersection
    else
      return intersection;

  }

  return NULL; //NOT_INTERSECTING;
}

ostream& operator <<(ostream &os, Edge &e) {
  os << "<edge power=\"" << e.power << "\" id=\"" << e.id << "\" >" << std::endl;
  os << *e.getStart();
  os << *e.getEnd();
  os << "</edge>" << std::endl;
  return os;
}
