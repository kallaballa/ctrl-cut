/*
 * Line.h
 *
 *  Created on: 26.09.2009
 *      Author: amir
 */

#ifndef LINE_H_
#define LINE_H_

#include "Point.h"



class LineSegment {
private:
	Point *start;
	Point *end;
	int power;
public:
	enum IntersectResult { PARALLEL, COINCIDENT, NOT_INTERSECTING, INTERSECTING };
	LineSegment(Point *start,Point *end, int power);
	virtual ~LineSegment();
	void setStart(Point* start);
	void setEnd(Point* end);
	Point* getStart();
	Point* getEnd();
	void setPower(int power);
	int getPower();
	float getSlope();
	IntersectResult intersects(LineSegment *otherLine, Point *intersection);
};


#endif /* LINE_H_ */
