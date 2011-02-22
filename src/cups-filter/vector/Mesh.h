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
#include <string>

using std::map;
using std::list;
using std::pair;

class Mesh {
public:
  typedef list<class Edge*> EdgeList;
  typedef EdgeList::iterator iterator;
  typedef EdgeList::const_iterator const_iterator;
  typedef map<std::string, class Vertex*> VertexMap;

  Mesh() {};
  virtual ~Mesh() {};

  iterator begin() { return this->edges.begin(); }
  const_iterator begin() const  { return this->edges.begin(); }
  iterator end() { return this->edges.end(); }
  const_iterator end() const  { return this->edges.end(); }
  size_t size() const { return this->edges.size(); }

  const VertexMap &getVertexMap() const {
    return this->vertices;
  }
  void create(int startX, int startY, int endX, int endY, 
              int power = -1, int speed = -1, int frequency = -1);
  void create(Vertex *start, Vertex *end,
              int power = -1, int speed = -1, int frequency = -1);
  void remove(Edge *e);
  iterator eliminate(iterator it);

private:
  VertexMap vertices;
  EdgeList edges;

  Vertex *mapVertex(Vertex *p);
};

std::ostream& operator <<(std::ostream &os, const Mesh &mesh);

#endif /* MESH_H_ */
