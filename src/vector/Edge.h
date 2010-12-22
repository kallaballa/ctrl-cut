#ifndef EDGE_H_
#define EDGE_H_

#include <set>
#include <list>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <iostream>
#include "Vertex.h"


class Edge {
 public:
	int pos;
  enum IntersectResult { PARALLEL, COINCIDENT, NOT_INTERSECTING, INTERSECTING };

  Edge(Vertex *start,Vertex *end, int power);
  virtual ~Edge();

  void setStart(Vertex* start);
  void setEnd(Vertex* end);
  Vertex* getStart();
  Vertex* getEnd();
  void setPower(int power);
  int getPower();
  float getSlope(bool invert=false);
  void invertDirection();
  Vertex* intersects(Edge *otherLine);
  void detach();

  friend ostream& operator <<(ostream &os, Edge &e);

 private:
  static int cnt;
  int id;
  Vertex *start;
  Vertex *end;
  int power;
};

#endif /* LINE_H_ */
