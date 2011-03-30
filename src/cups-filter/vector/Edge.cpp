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
#include "Vertex.h"

// initialize counter
int Edge::cnt = 0;

Edge::Edge(class Vertex *start, Vertex *end, int power, int speed, int frequency)
  : power(power), speed(speed), frequency(frequency)
{
  v[0] = start;
  v[1] = end;
  this->id = Edge::cnt++;
}

/*!
  Calculates the distance from the vertex to the infinite line segment defined by this edge
*/
float Edge::distance(const Vertex &p) const
{
  return 
    fabs(((*v[0])[1] - (*v[1])[1]) * p[0] + ((*v[1])[0] - (*v[0])[0]) * p[1] +
     (*v[0])[0] * (*v[1])[1] - (*v[1])[0] * (*v[0])[1]) /
    sqrt(((*v[1])[0] - (*v[0])[0]) * ((*v[1])[0] - (*v[0])[0]) +
         ((*v[1])[1] - (*v[0])[1]) * ((*v[1])[1] - (*v[0])[1]));
}

void Edge::invertDirection()
{
  Vertex* tmp = this->v[0];
  this->v[0] = this->v[1];
  this->v[1] = tmp;
}

void Edge::detach() {
  this->v[0]->detach(this);
  this->v[1]->detach(this);
}

void Edge::attach()
{
  this->v[0]->attach(this);
  this->v[1]->attach(this);
}

/*!
  Returns angle to the positive Y axis
 */
float Edge::getSlope(bool invert) {
  int d_x;
  int d_y;
  if (invert) {
    d_x = (*this)[0][0] - (*this)[1][0];
    d_y = (*this)[0][1] - (*this)[1][1];
  } else {
    d_x = (*this)[1][0] - (*this)[0][0];
    d_y = (*this)[1][1] - (*this)[0][1];
  }

  // Swap x and y since we're measuring relative to the Y axis.
  // We also negate the Y axis since positive Y points downwards (left handed)
  return atan2(d_x, -d_y);
}

/*!
  Intersection test.
  If the two edges intersect, returns a new vertex at the intersection point.

  Returns NULL on no intersection
*/
Vertex* Edge::intersects(const Edge &other) const
{
  float denom =
    ((other[1][1] - other[0][1]) * ((*this)[1][0] - (*this)[0][0])) -
    ((other[1][0] - other[0][0]) * ((*this)[1][1] - (*this)[0][1]));

  float nume_a =
    ((other[1][0] - other[0][0]) * ((*this)[0][1] - other[0][1])) -
    ((other[1][1] - other[0][1]) * ((*this)[0][0] - other[0][0]));

  float nume_b =
    (((*this)[1][0] - (*this)[0][0]) * ((*this)[0][1] - other[0][1])) -
    (((*this)[1][1] - (*this)[0][1]) * ((*this)[0][0] - other[0][0]));

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
    int ix = (*this)[0][0] + ua * ((*this)[1][0] - (*this)[0][0]);
    int iy = (*this)[0][1] + ua * ((*this)[1][1] - (*this)[0][1]);
    
    Vertex* intersection = new Vertex(ix, iy);
    if (((*this)[0] == *intersection || (*this)[1] == *intersection) &&
        (other[0] == *intersection || other[1] == *intersection)) {
      return NULL; //tip intersection
    }
    else {
      return intersection;
    }
  }

  return NULL; //NOT_INTERSECTING;
}

std::ostream &operator<<(std::ostream &os, const Edge &e) {
  os << "<edge power=\"" << e.power << "\" id=\"" << e.id << "\" >" << std::endl;
  os << e[0] << e[1];
  os << "</edge>" << std::endl;
  return os;
}
