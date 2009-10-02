/*
 * LSPoint.cpp
 *
 *  Created on: 26.09.2009
 *      Author: amir
 */

#include <set>
#include <string>
#include <sstream>
#include <iostream>
#include "LineSegment.h"

LineSegment::LSPoint::LSPoint(int x, int y) {
	setX(x,false);
	setY(y);
}

void LineSegment::LSPoint::updateKey()
{
	this->key = "";

	std::stringstream out;
	out << this->x;
	this->key += out.str();
	out.str("");
	this->key += "/";
	out << this->y;
	this->key += out.str();
}

LineSegment::LSPoint::~LSPoint() {
	// TODO Auto-generated destructor stub
}

void LineSegment::LSPoint::addConnector(LineSegment* ls)
{
	connectors.insert(ls);
}

void LineSegment::LSPoint::removeConnector(LineSegment* ls)
{
	connectors.erase(ls);
}

std::set<LineSegment*> LineSegment::LSPoint::getConnectors()
{
	return connectors;
}

int LineSegment::LSPoint::getX() {
	return this->x;
}

int LineSegment::LSPoint::getY() {
	return this->y;
}

void LineSegment::LSPoint::setX(int x) {
	setX(x,true);
}

void LineSegment::LSPoint::setY(int y) {
	setY(y,true);
}

void LineSegment::LSPoint::setX(int x, bool update) {
	this->x = x;
	if(update)
		updateKey();
}

void LineSegment::LSPoint::setY(int y, bool update) {
	this->y = y;
	if(update)
		updateKey();
}

std::string LineSegment::LSPoint::getKey()
{
	return key;
}

bool LineSegment::LSPoint::equals(LSPoint *other) {
  return x == other->x && y == other->y;
}

