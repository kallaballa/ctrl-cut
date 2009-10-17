/*
 * VectorCut.h
 *
 *  Created on: 04.10.2009
 *      Author: amir
 */

#ifndef VECTORCUT_H_
#define VECTORCUT_H_

#include <list>
#include <map>
#include <string>
#include "LineSegment.h"

using namespace std;

class VectorCut {
private:
	//TODO: super inefficent string based key comparator
	map<string, Joint*> points;
	list<LineSegment*> lines;

public:
	VectorCut();
	virtual ~VectorCut();

	Joint* addPoint(Joint* p);

	void addLine(Joint *start, Joint *end, int power);
	list<LineSegment*>::iterator eraseLine(list<LineSegment*>::iterator it_ls);

	map<string, Joint*> getPoints();
	list<LineSegment*> getLineSegements();
	void load(const string *file_name);
};

#endif /* VECTORCUT_H_ */
