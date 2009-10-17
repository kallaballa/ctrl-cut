/*
 * VectorCut.cpp
 *
 *  Created on: 04.10.2009
 *      Author: amir
 */

#include "VectorCut.h"
#include <fstream>
#include <iostream>

VectorCut::VectorCut() {
}

VectorCut::~VectorCut() {
	// TODO Auto-generated destructor stub
}

map<string, Joint*> VectorCut::getPoints()
{
	return this->points;
}

list<LineSegment*> VectorCut::getLineSegements()
{
	return this->lines;
}

Joint* VectorCut::addPoint(Joint* p) {
	map<string, Joint*>::iterator it = points.find(p->getKey());

	if (it != points.end()) {
		return (Joint*) it->second;
	}

	points.insert(pair<string, Joint*> (p->getKey(), p));
	return p;
}



void VectorCut::addLine(Joint *start, Joint *end, int power) {
	LineSegment *ls = new LineSegment(start, end, power);

	start = addPoint(start);
	ls->setStart(start);
	start->addConnector(ls);

	end = addPoint(end);
	ls->setEnd(end);
	end->addConnector(ls);

	lines.push_back(ls);
}

list<LineSegment*>::iterator VectorCut::eraseLine(list<LineSegment*>::iterator it_ls) {
	LineSegment *ls = *it_ls;
	ls->getStart()->removeConnector(ls);
	ls->getEnd()->removeConnector(ls);

	return lines.erase(it_ls);
}

void VectorCut::load(const string *file_name)
{
	string line;
	ifstream infile(file_name->data(), ios_base::in);
	char first;
	int power, x, y;
	int lx, ly;
	int mx, my;
	Joint *start;
	Joint *end;

	while (getline(infile, line, '\n')) {
		first = *line.begin();

		if (first == 'X')
			break;

		if (isalpha(first)) {
			switch (first) {
			case 'M': // move
				if (sscanf((char *) line.data() + 1, "%d,%d", &y, &x) == 2) {
					lx = x;
					ly = y;
					mx = x;
					my = y;
				}
				break;
			case 'C': // close
				if (lx != mx || ly != my) {
					addLine(new Joint(lx, ly),
							new Joint(mx, my), power);
				}
				break;
			case 'P': // power
				if (sscanf((char *) line.data() + 1, "%d", &x) == 1) {
					power = x;
				}
				break;
			case 'L': // line
				if (sscanf((char *) line.data() + 1, "%d,%d", &y, &x) == 2) {
					start = new Joint(lx, ly);
					end = new Joint(x, y);
					addLine(start, end, power);
					lx = x;
					ly = y;
				}
				break;
			}
		}
	}
	infile.close();
}

