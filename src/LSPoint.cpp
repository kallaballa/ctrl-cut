/*
 * LSPoint.cpp
 *
 *  Created on: 26.09.2009
 *      Author: amir
 */

#include <set>
#include "LSPoint.h"



LSPoint::LSPoint(int x, int y)
: Point(x,y) {

}

LSPoint::~LSPoint() {
	// TODO Auto-generated destructor stub
}

void LSPoint::addConnector(LineSegment* ls)
{
	connectors.insert(ls);
}

void LSPoint::removeConnector(LineSegment* ls)
{
	connectors.erase(ls);
}

std::set<LineSegment*> LSPoint::getConnectors()
{
	return connectors;
}
