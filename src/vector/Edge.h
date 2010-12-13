#ifndef LINE_H_
#define LINE_H_

#include <set>
#include <string>
#include <cmath>
#include "Vertex.h"

using namespace std;

class Edge {
 public:

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

 private:
  Vertex *start;
  Vertex *end;
  int power;
};

#endif /* LINE_H_ */
