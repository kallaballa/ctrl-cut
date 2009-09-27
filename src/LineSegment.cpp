/*
 * Line.cpp
 *
 *  Created on: 26.09.2009
 *      Author: amir
 */

#include "Point.h"
#include "LineSegment.h"
#include <algorithm>

using namespace std;

LineSegment::LineSegment(Point *start, Point *end, int power) {
	this->start = start;
	this->end = end;
	this->power = power;
}

LineSegment::~LineSegment() {
	// TODO Auto-generated destructor stub
}

Point* LineSegment::getStart() {
	return this->start;
}

Point* LineSegment::getEnd() {
	return this->end;
}

void LineSegment::setStart(Point* start) {
	this->start = start;
}

void LineSegment::setEnd(Point* end) {
	this->end = end;
}

void LineSegment::setPower(int power)
{
	this->power = power;
}
int LineSegment::getPower()
{
	return this->power;
}

LineSegment::IntersectResult LineSegment::intersects(LineSegment *otherLine,
		Point *intersection) {
	float denom = ((otherLine->end->y - otherLine->start->y) * (this->end->x
			- this->start->x)) - ((otherLine->end->x - otherLine->start->x)
			* (this->end->y - this->start->y));

	float nume_a = ((otherLine->end->x - otherLine->start->x) * (this->start->y
			- otherLine->start->y))
			- ((otherLine->end->y - otherLine->start->y) * (this->start->x
					- otherLine->start->x));

	float nume_b = ((this->end->x - this->start->x) * (this->start->y
			- otherLine->start->y)) - ((this->end->y - this->start->y)
			* (this->start->x - otherLine->start->x));

	if (denom == 0.0f) {
		if (nume_a == 0.0f && nume_b == 0.0f) {
			return COINCIDENT;
		}
		return PARALLEL;
	}

	float ua = nume_a / denom;
	float ub = nume_b / denom;

	if (ua >= 0.0f && ua <= 1.0f && ub >= 0.0f && ub <= 1.0f) {
		// Get the intersection point.
		int min_x = min(this->start->x, this->end->x);
		int max_x = max(this->start->x, this->end->x);
		int min_y = min(this->start->y, this->end->y);
		int max_y = max(this->start->y, this->end->y);

		int min_ox = min(otherLine->start->x, otherLine->end->x);
		int max_ox = max(otherLine->start->x, otherLine->end->x);
		int min_oy = min(otherLine->start->y, otherLine->end->y);
		int max_oy = max(otherLine->start->y, otherLine->end->y);

		int ix = this->start->x + ua * (this->end->x - this->start->x);
		int iy = this->start->y + ua * (this->end->y - this->start->y);

		if (ix > min_x && ix > min_ox && ix < max_x && ix < max_ox && iy
				> min_y && iy > min_oy && iy < max_y && iy < max_oy) {
			intersection->x = ix;
			intersection->y = iy;
			return INTERSECTING;
		}

	}

	return NOT_INTERSECTING;
}
