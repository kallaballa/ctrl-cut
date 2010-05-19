/*
 * OnionSkin.h
 *
 *  Created on: 03.10.2009
 *      Author: amir
 */

#ifndef ONIONSKIN_H_
#define ONIONSKIN_H_

#include <list>
#include "Polyline.h"

class OnionSkin: public Polyline {
 public:
  list<LineSegment*> segments;
  OnionSkin();
  virtual ~OnionSkin();
  void addLineSegment(LineSegment* ls);
  list<LineSegment*> getLineSegments();
  list<LineSegment*>::iterator findLineSegment(LineSegment* ls);
  void eraseLineSegment(LineSegment* ls);
  bool hasLineSegment(LineSegment* ls);
  int getSegmentCount();
};

#endif /* ONIONSKIN_H_ */
