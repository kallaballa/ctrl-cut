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

#include "Mesh.h"

Mesh::Mesh() {
  // TODO Auto-generated constructor stub

}

Mesh::~Mesh() {
  // TODO Auto-generated destructor stub
}

/*!
  Adds the given edge to the vector pass.
  power is the laser power in percent.

  FIXME: Speed and frequency should be specified as well
*/
void Mesh::create(Vertex *start, Vertex *end, int power)
{
  //filter zero length edges
  if(start->getKey() == end->getKey())
    return;

  // FIXME: Clip against page size
  if (start->getX() < 0 || start->getY() < 0 ||
      end->getX() < 0 || end->getY() < 0) {

    if (start->getX() < 0) start->setX(0);
    if (start->getY() < 0) start->setY(0);
    if (end->getX() < 0) end->setX(0);
    if (end->getY() < 0) end->setY(0);

    // FIXME: The Windows driver subtracts 1 point from the X
    // coordinate of the end of any line segment which is
    // clipped. Strange, but let's follow suit for now.
    end->setX(end->getX()-1);
  }

  Edge *ls = new Edge(start, end, power);

  start = mapVertex(start);
  ls->setStart(start);
  start->attach(ls);

  end = mapVertex(end);
  ls->setEnd(end);
  end->attach(ls);

  this->std::list<Edge *>::push_back(ls);
}

void Mesh::remove(Edge* e) {
  this->std::list<Edge *>::remove(e);
}

Mesh::iterator Mesh::eliminate(Mesh::iterator it_e) {
  (*it_e)->detach();
  return this->std::list<Edge *>::erase(it_e);
}

Vertex* Mesh::mapVertex(Vertex *p)
{
  map<string, Vertex* >::iterator it = vertices.find(p->getKey());

  if (it != vertices.end()) {
    return (Vertex *)it->second;
  }

  vertices.insert(pair<string, Vertex*> (p->getKey(), p));
  return p;
}

ostream& operator<< (ostream &os, Mesh &mesh) {
  os << "<mesh>" << std::endl;
  os << "<edges cnt=\"" << mesh.size() << "\" >" << std::endl;
  for(Mesh::iterator it = mesh.begin(); it != mesh.end(); it++) {
    os << *((Edge*)*it);
  }
  os << "</edges>" << std::endl;
  map<string, Vertex* >& vertices = mesh.getVertexMap();

  os << "<vertices cnt=\"" << vertices.size() << "\" >" << std::endl;
  for (map<string, Vertex*>::iterator it = vertices.begin(); it != vertices.end(); it++) {
    Vertex* vec = (Vertex*) (*it).second;
    os << *vec;
  }
  os << "</vertices>" << std::endl;
  os << "</mesh>" << std::endl;

  return os;
}
