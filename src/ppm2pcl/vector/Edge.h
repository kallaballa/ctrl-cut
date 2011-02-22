#ifndef EDGE_H_
#define EDGE_H_

#include <map>
#include <string>
#include <cmath>
#include <algorithm>
#include <iostream>

class Edge {
public:
  class Vertex* start;
  Vertex* end;
  int power;
  int speed;
  int frequency;
  enum IntersectResult { PARALLEL, COINCIDENT, NOT_INTERSECTING, INTERSECTING };

  float getSlope(bool invert=false);
  void invertDirection();
  Vertex *intersects(Edge *otherLine);
  void detach();

  friend std::ostream &operator<<(std::ostream &os, const Edge &e);

  Edge(Vertex *start, Vertex *end, int power=0, int speed=0, int frequency=0) : start(start), end(end), power(power), speed(speed), frequency(frequency) {
    this->id = cnt++;
  }
  virtual ~Edge() {}
private:
  static int cnt;
  int id;
};

#endif /* LINE_H_ */
