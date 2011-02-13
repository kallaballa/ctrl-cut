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
#include <set>
#include <string>
#include <sstream>
#include <iostream>
#include "Edge.h"

int Vertex::cnt = 0;

Vertex::Vertex(int x, int y) {
  this->id = cnt++;
  setX(x, false);
  setY(y);
}

void Vertex::updateKey() {
  this->key = "";

  std::stringstream out;
  out << this->x;
  this->key += out.str();
  out.str("");
  this->key += "/";
  out << this->y;
  this->key += out.str();
}

Vertex::~Vertex() {
}

void Vertex::attach(Edge* ls) {
  this->std::set<Edge*>::insert(ls);
}

void Vertex::detach(Edge* ls) {
  this->std::set<Edge*>::erase(ls);
}


int Vertex::getX() {
  return this->x;
}

int Vertex::getY() {
  return this->y;
}

void Vertex::setX(int x, bool update) {
  this->x = x;
  if (update)
    updateKey();
}

void Vertex::setY(int y, bool update) {
  this->y = y;
  if (update)
    updateKey();
}

std::string Vertex::getKey() {
  return this->key;
}

bool Vertex::equals(Vertex *other) {
  return x == other->x && y == other->y;
}

ostream& operator <<(ostream &os,const Vertex &v) {
  os << "<vertex x=\"" << v.x << "\" y=\"" << v.y << "\" id=\"" << v.id
     << "\" key=\"" << v.key << "\" />" << std::endl;
  return os;
}
