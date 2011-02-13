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

#ifndef MESH_H_
#define MESH_H_

#include <map>
#include <list>
#include "Vertex.h"
#include "Edge.h"

using std::map;
using std::list;
using std::pair;

class Mesh : public list<Edge *> {
public:
  map<string, Vertex*>& getVertexMap(){
    return this->vertices;
  }

  void create(Vertex *start, Vertex *end, int power = 0, int speed = 0, int frequency = 0);
  void remove(Edge *e);
  Mesh::iterator eliminate(Mesh::iterator it_e);

  Mesh() {};
  virtual ~Mesh() {};

  friend std::ostream& operator <<(std::ostream &os, Mesh &mesh);
private:
  map<string, Vertex* > vertices;

  Vertex* mapVertex(Vertex* p);
};

#endif /* MESH_H_ */
