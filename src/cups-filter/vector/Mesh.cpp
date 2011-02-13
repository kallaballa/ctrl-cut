/*
 * Mesh.cpp
 *
 *  Created on: Feb 13, 2011
 *      Author: elchaschab
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
void Mesh::createEdge(Vertex *start, Vertex *end, int power)
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

void Mesh::removeEdge(Edge* e, bool detach) {
  if(detach)
    e->detach();
  this->std::list<Edge *>::remove(e);
}

Mesh::iterator Mesh::removeEdge(Mesh::iterator it_e, bool detach) {
  Edge *e = *it_e;
  if(detach)
    e->detach();

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
