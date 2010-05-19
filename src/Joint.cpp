#include <set>
#include <string>
#include <sstream>
#include <iostream>
#include "LineSegment.h"

Joint::Joint(int x, int y) {
  setX(x, false);
  setY(y);
}

void Joint::updateKey() {
  this->key = "";

  std::stringstream out;
  out << this->x;
  this->key += out.str();
  out.str("");
  this->key += "/";
  out << this->y;
  this->key += out.str();
}

Joint::~Joint() {
  // TODO Auto-generated destructor stub
}

void Joint::addConnector(LineSegment* ls) {
  connectors.insert(ls);
}

void Joint::removeConnector(LineSegment* ls) {
  connectors.erase(ls);
}

std::set<LineSegment*> Joint::getConnectors() {
  return connectors;
}

int Joint::getX() {
  return this->x;
}

int Joint::getY() {
  return this->y;
}

void Joint::setX(int x, bool update) {
  this->x = x;
  if (update)
    updateKey();
}

void Joint::setY(int y, bool update) {
  this->y = y;
  if (update)
    updateKey();
}

std::string Joint::getKey() {
  return key;
}

bool Joint::equals(Joint *other) {
  return x == other->x && y == other->y;
}

