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
#include "LaserJob.h"
#include "LineSegment.h"
#include "OnionSkin.h"

using namespace std;

class VectorPass : public LaserPass{
private:
	//TODO: super inefficent string based key comparator
	map<string, Joint*> joints;
	list<LineSegment*> lines;
	vector<OnionSkin*> skins;

public:
	VectorPass(pass_params params) : LaserPass(params)	{};
	virtual ~VectorPass();

	Joint* addJoint(Joint* p);
	map<string, Joint*> getJoints();

	void addLine(Joint *start, Joint *end, int power);
	list<LineSegment*>::iterator eraseLine(list<LineSegment*>::iterator it_ls);
	list<LineSegment*> getLineSegements();

	vector<OnionSkin*> getOnionSkins();
	void addOnionSkin(OnionSkin* skin);


	void load(const string *file_name);
	void write(const string* file_name);
};

#endif /* VECTORCUT_H_ */
