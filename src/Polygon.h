/*
 * Polygon.h
 *
 *  Created on: 01.10.2009
 *      Author: amir
 */

#ifndef POLYGON_H_
#define POLYGON_H_
#include "LineSegment.h"
#include <vector>
using namespace std;

class Polygon {
 public:
  set<LineSegment*> segments;
  Polygon();
  virtual ~Polygon();
  void addLineSegment(LineSegment* ls);
  set<LineSegment*> getLineSegments();
  void eraseLineSegment(LineSegment* ls);
  bool hasLineSegment(LineSegment* ls);
  int getSegmentCount();
  LineSegment* findEdge();
};

#endif /* POLYGON_H_ */
