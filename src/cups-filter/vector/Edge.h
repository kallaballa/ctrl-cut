#ifndef EDGE_H_
#define EDGE_H_

#include <map>
#include <string>
#include <cmath>
#include <algorithm>
#include <iostream>

class Edge {
public:
  Edge(class Vertex *start, Vertex *end, int power=0, int speed=0, int frequency=0)
    : start(start), end(end), power(power), speed(speed), frequency(frequency) {
    this->id = Edge::cnt++;
  }
  virtual ~Edge() {}

  Vertex *start;
  Vertex *end;
  int power;
  int speed;
  int frequency;

  float getSlope(bool invert=false);
  void invertDirection();
  Vertex *intersects(const Edge &other) const;
  void detach();

private:
  static int cnt;
  int id;

  friend std::ostream &operator<<(std::ostream &os, const Edge &e);
};


#endif
