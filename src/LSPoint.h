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
#include <set>

class LSPoint: public Point {
private:
	std::set<LineSegment*> connectors;
public:
	LSPoint(int x, int y);
	virtual ~LSPoint();
	void addConnector(LineSegment *ls);
	void removeConnector(LineSegment *ls);
	std::set<LineSegment*> getConnectors();

};

#endif /* LSPOINT_H_ */
