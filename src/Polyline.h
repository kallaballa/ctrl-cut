#ifndef polyline_H_
#define polyline_H_

#include "LineSegment.h"
#include <set>

using std::set;

class Polyline {
 public:
  set<LineSegment*> segments;
  Polyline();
  virtual ~Polyline();
  void addLineSegment(LineSegment* ls);
  set<LineSegment*> getLineSegments();
  void eraseLineSegment(LineSegment* ls);
  bool hasLineSegment(LineSegment* ls);
  int getSegmentCount();
  LineSegment* findEdge();
};

#endif /* polyline_H_ */
