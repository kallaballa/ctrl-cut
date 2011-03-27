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
#include "Vertex.h"
#include "Edge.h"

void Mesh::create(int startX, int startY, int endX, int endY, int power, int speed, int frequency)
{
  this->create(new Vertex(startX, startY), new Vertex(endX, endY), power, speed, frequency);
}

/*
 * Adds the given edge to the vector pass.
 */
void Mesh::create(Vertex *start, Vertex *end, int power, int speed, int frequency)
{
  //filter zero length edges
  if (start->getKey() == end->getKey()) {
    return;
  }

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

  start = mapVertex(start);
  end = mapVertex(end);

  Edge *edge = new Edge(start, end, power, speed, frequency);
  edge->attach();

  this->edges.push_back(edge);
}

void Mesh::remove(Edge* e)
{
  this->edges.remove(e);
}

Mesh::iterator Mesh::eliminate(Mesh::iterator it)
{
  (*it)->detach();
  return this->edges.erase(it);
}

void Mesh::clear()
{
  // FIXME: Memory leaks, but we need to keep a subset of the vertices..
  this->vertices.clear();
  this->edges.clear();
}

Vertex *Mesh::mapVertex(Vertex *p)
{
  VertexMap::iterator it = vertices.find(p->getKey());

  if (it != vertices.end()) {
    return it->second;
  }

  vertices[p->getKey()] = p;
  return p;
}

std::ostream &operator<<(std::ostream &os, const Mesh &mesh) {
  os << "<mesh>" << std::endl;
  os << "<edges cnt=\"" << mesh.size() << "\" >" << std::endl;
  for(Mesh::EdgeList::const_iterator it = mesh.begin(); it != mesh.end(); it++) {
    os << **it;
  }
  os << "</edges>" << std::endl;
  const Mesh::VertexMap &vertices = mesh.getVertexMap();

  os << "<vertices cnt=\"" << vertices.size() << "\" >" << std::endl;
  for (Mesh::VertexMap::const_iterator it = vertices.begin(); it != vertices.end(); it++) {
    Vertex* vec = (Vertex*) (*it).second;
    os << *vec;
  }

  os << "</vertices>" << std::endl;
  os << "</mesh>" << std::endl;

  return os;
}
