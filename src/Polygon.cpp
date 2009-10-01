/*
 * Polygon.cpp
 *
 *  Created on: 01.10.2009
 *      Author: amir
 */
#include <vector>
#include "LineSegment.h"
#include "Polygon.h"

Polygon::Polygon() {
	// TODO Auto-generated constructor stub

}

Polygon::~Polygon() {
	// TODO Auto-generated destructor stub
}

void Polygon::addLineSegment(LineSegment* ls) {
	this->segments.push_back(ls);
}

vector<LineSegment*> Polygon::getLineSegments() {
	return this->segments;
}

int Polygon::getSegmentCount() {
	return this->segments.size();
}
