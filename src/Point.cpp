/*
 * Point.cpp
 *
 *  Created on: 26.09.2009
 *      Author: amir
 */

#include <string>
#include <sstream>
#include "Point.h"

Point::Point(int x, int y) {
	this->x = x;
	this->y = y;

	std::stringstream out;
	out << x;
	this->key += out.str();
	this->key += "/";
	out << y;
	this->key += out.str();
}


int Point::getX() {
	return this->x;
}

int Point::getY() {
	return this->y;
}

Point::~Point() {
	// TODO Auto-generated destructor stub
}

std::string Point::getKey()
{
	return key;
}

bool Point::equals(Point *other) {
  return x == other->x && y == other->y;
}
