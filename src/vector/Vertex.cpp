#include <set>
#include <string>
#include <sstream>
#include <iostream>
#include "Edge.h"

Vertex::Vertex(int x, int y) {
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
  // TODO Auto-generated destructor stub
}

void Vertex::attach(Edge* ls) {
  attachedEdges.insert(ls);
}

void Vertex::detach(Edge* ls) {
  attachedEdges.erase(ls);
}

std::set<Edge*> Vertex::getAttachedEdges() {
  return attachedEdges;
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
  return key;
}

bool Vertex::equals(Vertex *other) {
  return x == other->x && y == other->y;
}

