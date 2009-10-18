/*
 * LaserJob.h
 *
 *  Created on: 18.10.2009
 *      Author: amir
 */

#ifndef LASERJOB_H_
#define LASERJOB_H_

#include <list>
#include <map>
#include <string>
#include "laser_config.h"
#include "LineSegment.h"
#include "OnionSkin.h"
#include "PrinterConnection.h"

using namespace std;

//stub
struct pass_params {
};

class LaserPass {
	pass_params params;

public:
	LaserPass(){};
	LaserPass(pass_params params){this->params = params;};
	virtual ~LaserPass(){};
};


class LaserJob {
	list<LaserPass*> passes;
	laser_config *lconf;
	printer_config *pconf;
public:
	LaserJob(laser_config *lconf, printer_config *pconf);
	virtual ~LaserJob();

	void addPass(LaserPass* pass);
};


class VectorPass : public LaserPass{
private:
	//TODO: super inefficent string based key comparator
	map<string, Joint*> joints;
	list<LineSegment*> lines;
	vector<OnionSkin*> skins;

public:
	VectorPass(){};
	VectorPass(pass_params params) : LaserPass(params)	{};
	virtual ~VectorPass(){};

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

class RasterPass: public LaserPass {
public:
	RasterPass(){};
	RasterPass(pass_params params) : LaserPass(params){};
	virtual ~RasterPass(){};
};

#endif /* LASERJOB_H_ */
