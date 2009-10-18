/*
 * LaserJob.h
 *
 *  Created on: 18.10.2009
 *      Author: amir
 */

#ifndef LASERJOB_H_
#define LASERJOB_H_

#include <list>

using namespace std;

//stub
struct pass_params {
};

class LaserPass {
	pass_params params;

public:
	LaserPass(pass_params params){this->params = params;};
	virtual ~LaserPass();
};


class LaserJob {
	list<LaserPass*> passes;
public:
	LaserJob(laser_config config);
	virtual ~LaserJob();

	void addPass(LaserPass pass);
};

#endif /* LASERJOB_H_ */
