/*
 * Point.cpp
 *
 *  Created on: 26.09.2009
 *      Author: amir
 */

#include "Point.h"

Point::Point(int x, int y) {
	this->x = x;
	this->y = y;

}

void Point::set(int x, int y) {
	this->x = x;
	this->y = y;

}

Point::~Point() {
	// TODO Auto-generated destructor stub
}

bool Point::equals(Point *other) {
  return x == other->x && y == other->y;
}
