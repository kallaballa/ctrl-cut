/*
 * LSPoint.h
 *
 *  Created on: 26.09.2009
 *      Author: amir
 */

#ifndef LSPOINT_H_
#define LSPOINT_H_

#include "Point.h"
#include "LineSegment.h"
#include <vector>
using namespace std;

class LSPoint: public Point {
private:
	vector<LineSegment*> parents;
public:
	LSPoint(int x, int y);
	virtual ~LSPoint();
	void addParent(LineSegment *ls);
	void removeParent(LineSegment *ls);
};

#endif /* LSPOINT_H_ */
