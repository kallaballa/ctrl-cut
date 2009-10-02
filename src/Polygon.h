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
private:
	vector<LineSegment*> segments;
public:
	Polygon();
	virtual ~Polygon();
	void addLineSegment(LineSegment* ls);
	vector<LineSegment*> getLineSegments();
	int getSegmentCount();
};

#endif /* POLYGON_H_ */
