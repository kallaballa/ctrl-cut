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
#include "util/LaserConfig.h"
#include <assert.h>

Edge *Mesh::create(int startX, int startY, int endX, int endY, int power, int speed, int frequency)
{
  return this->create(new Vertex(startX, startY), new Vertex(endX, endY), power, speed, frequency);
}

/*!
  Clips the given edge against the laserbed.
  FIXME: Currently only clips against the x and y axes, not the maximum axes.
 */
static void clip(Edge *edge)
{
  bool clipped = false;
  Vertex origin(0, 0);
  Vertex xmax(LaserConfig::inst().device_width, 0);
  Vertex ymax(0, LaserConfig::inst().device_height);
  Edge xaxis(&origin, &xmax);
  Edge yaxis(&origin, &ymax);
  // intersect against Y axis
  if ((*edge)[0][0] < 0 || (*edge)[1][0] < 0) {
    Vertex *intersect = edge->intersects(yaxis);
    if (intersect) {
      if ((*edge)[0][0] < 0) (*edge)[0] = *intersect;
      else (*edge)[1] = *intersect;
      clipped = true;
    }
  }
  // intersect against X axis
  if ((*edge)[0][1] < 0 || (*edge)[1][1] < 0) {
    Vertex *intersect = edge->intersects(xaxis);
    if (intersect) {
      if ((*edge)[0][1] < 0) (*edge)[0] = *intersect;
      else (*edge)[1] = *intersect;
      clipped = true;
    }
  }

  assert((*edge)[0][0] >= 0 && (*edge)[0][1] >= 0 &&
         (*edge)[1][0] >= 0 && (*edge)[1][1] >= 0);

  // FIXME: The Windows driver subtracts 1 point from the X
  // coordinate of the end of any line segment which is
  // clipped. Strange, but let's follow suit for now.
  // FIXME FIXME: This behavior is not reproducible with the current test cases.
  // Before putting this back, verify the original assumption again. kintel 20110426.
  // if (clipped && (*edge)[1][0] > 0) (*edge)[1][0] -= 1;
}

/*
 * Adds the given edge to the vector pass.
 */
Edge *Mesh::create(Vertex *start, Vertex *end, int power, int speed, int frequency)
{
  //filter zero length edges
  if (start->getKey() == end->getKey()) {
    return NULL;
  }

  start = mapVertex(start);
  end = mapVertex(end);

  Edge *edge = new Edge(start, end, power, speed, frequency);

  clip(edge);

  edge->attach();

  this->edges.push_back(edge);
  return edge;
}

void Mesh::remove(Edge* e)
{
  this->edges.remove(e);
}

Mesh::iterator Mesh::eliminate(Mesh::iterator& it)
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
