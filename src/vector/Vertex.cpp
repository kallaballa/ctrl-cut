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
  return this->key;
}

bool Vertex::equals(Vertex *other) {
  return x == other->x && y == other->y;
}

ostream& operator <<(ostream &os,const Vertex &v) {
	os << "<vertex x=\"" << v.x << "\" y=\"" << v.y << "\" id=\"" << v.id << "\" key=\"" << v.key << "\" />" << std::endl;
	return os;
}
