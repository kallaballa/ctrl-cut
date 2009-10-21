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
	printer_config *pconf;
	laser_config *lconf;
	LaserPass(){};
	LaserPass(pass_params params){this->params = params;};
	virtual ~LaserPass(){};
};


class LaserJob {
public:
	list<LaserPass*> passes;
	printer_config *pconf;
	laser_config *lconf;

	LaserJob(laser_config *lconf, printer_config *pconf);
	virtual ~LaserJob();

	void addPass(LaserPass* pass);
	list<LaserPass*> getPasses();
};


class VectorPass : public LaserPass{
public:
	//TODO: super inefficent string based key comparator
	map<string, Joint*> joints;
	list<LineSegment*> lines;
	vector<OnionSkin*> skins;


	VectorPass(){};
	VectorPass(pass_params params) : LaserPass(params)	{};
	virtual ~VectorPass(){};

	Joint* addJoint(Joint* p);

	void addLine(Joint *start, Joint *end, int power);
	list<LineSegment*>::iterator eraseLine(list<LineSegment*>::iterator it_ls);

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
