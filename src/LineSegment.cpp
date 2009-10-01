/*
 * Line.cpp
 *
 *  Created on: 26.09.2009
 *      Author: amir
 */

#include "Point.h"
#include "LineSegment.h"
#include <algorithm>
#include <cmath>

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


float LineSegment::getSlope() {
	int d_x =  end->getX() - start->getX();
	int d_y = end->getY() - start->getY();


	return atan2(d_y, d_x);
}

LineSegment::IntersectResult LineSegment::intersects(LineSegment *otherLine,
		Point *intersection) {
	float denom = ((otherLine->end->getY() - otherLine->start->getY()) * (this->end->getX()
			- this->start->getX())) - ((otherLine->end->getX() - otherLine->start->getX())
			* (this->end->getY() - this->start->getY()));

	float nume_a = ((otherLine->end->getX() - otherLine->start->getX()) * (this->start->getY()
			- otherLine->start->getY()))
			- ((otherLine->end->getY() - otherLine->start->getY()) * (this->start->getX()
					- otherLine->start->getX()));

	float nume_b = ((this->end->getX() - this->start->getX()) * (this->start->getY()
			- otherLine->start->getY())) - ((this->end->getY() - this->start->getY())
			* (this->start->getX() - otherLine->start->getX()));

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
		int min_x = min(this->start->getX(), this->end->getX());
		int max_x = max(this->start->getX(), this->end->getX());
		int min_y = min(this->start->getY(), this->end->getY());
		int max_y = max(this->start->getY(), this->end->getY());

		int min_ox = min(otherLine->start->getX(), otherLine->end->getX());
		int max_ox = max(otherLine->start->getX(), otherLine->end->getX());
		int min_oy = min(otherLine->start->getY(), otherLine->end->getY());
		int max_oy = max(otherLine->start->getY(), otherLine->end->getY());

		int ix = this->start->getX() + ua * (this->end->getX() - this->start->getX());
		int iy = this->start->getY() + ua * (this->end->getY() - this->start->getY());

		if (ix >= min_x && ix >= min_ox && ix <= max_x && ix <= max_ox && iy
				>= min_y && iy >= min_oy && iy <= max_y && iy <= max_oy) {


			intersection = new Point(ix,iy);
			return INTERSECTING;
		}

	}

	return NOT_INTERSECTING;
}
