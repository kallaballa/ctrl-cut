/*
 * LSPoint.cpp
 *
 *  Created on: 26.09.2009
 *      Author: amir
 */

#include "LSPoint.h"
#include <vector>

LSPoint::LSPoint(int x, int y)
: Point(x,y) {

}

LSPoint::~LSPoint() {
	// TODO Auto-generated destructor stub
}

void LSPoint::addParent(LineSegment* ls)
{
	parents.push_back(ls);
}

void LSPoint::removeParent(LineSegment* ls)
{
	unsigned int i;

	for(i = 0; i < parents.size(); i++)
	{
		if(parents.at(i) == ls)	{
			parents.erase(parents.begin() + i);
			break;
		}
	}
}
