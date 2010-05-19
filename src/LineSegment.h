#ifndef LINE_H_
#define LINE_H_

#include <set>
#include <string>
#include <cmath>
#include "Joint.h"

using namespace std;

class LineSegment {
 public:

  enum IntersectResult { PARALLEL, COINCIDENT, NOT_INTERSECTING, INTERSECTING };
  LineSegment(Joint *start,Joint *end, int power);
  virtual ~LineSegment();
  void setStart(Joint* start);
  void setEnd(Joint* end);
  Joint* getStart();
  Joint* getEnd();
  void setPower(int power);
  int getPower();
  float getSlope(bool invert=false);
  void invertDirection();
  Joint* intersects(LineSegment *otherLine);

 private:
  Joint *start;
  Joint *end;
  int power;
};

#endif /* LINE_H_ */
