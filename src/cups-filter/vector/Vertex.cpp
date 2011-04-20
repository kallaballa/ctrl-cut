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
#include "Vertex.h"
#include "Edge.h"

#include <assert.h>
#include <set>
#include <string>
#include <sstream>
#include <iostream>

int Vertex::cnt = 0;

Vertex::Vertex(int x, int y) : Point2D(x,y)
{
  assert(x < 21600);
  assert(y < 14400);
  assert(x > -1);
  assert(y > -1);

  this->id = cnt++;
  updateKey();
}

void Vertex::updateKey() {
  this->key = "";

  std::stringstream out;
  out << this->v[0];
  this->key += out.str();
  out.str("");
  this->key += "/";
  out << this->v[1];
  this->key += out.str();
}

void Vertex::attach(Edge* ls) {
  this->edges.insert(ls);
}

void Vertex::detach(Edge* ls) {
  this->edges.erase(ls);
}

const std::string &Vertex::getKey() const {
  return this->key;
}

int Vertex::distance(const Vertex &other) {
  return hypot(abs(this->v[0] - other.v[0]), abs(this->v[1] - other.v[1]));
}

std::ostream &operator<<(std::ostream &os, const Vertex &v) {
  os << "<vertex x=\"" << v[0] << "\" y=\"" << v[1] << "\" id=\"" << v.getID()
     << "\" key=\"" << v.getKey() << "\" />" << std::endl;
  return os;
}
