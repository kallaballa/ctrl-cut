/*
 * Driver.cpp
 *
 *  Created on: 18.10.2009
 *      Author: amir
 */

#include "Driver.h"
#include "FilterChain.h"
#include "LaserJob.h"
#include "LaserPass.h"
#include <iostream>
#include <fstream>
#include <sstream>

Driver::Driver() {
	// TODO Auto-generated constructor stub

}

Driver::~Driver() {
	// TODO Auto-generated destructor stub
}

void Driver::filter(LaserJob *job) {
	FilterChain fc;

	if (!fc.evaluate()) {
		std::cerr << "Filter chain evaluation failed" << endl;
	}

	LaserPassList &passes = job->getPasses();

	LaserPassList::iterator it;
	VectorPass *vpass;
	RasterPass *rpass;

	for (it = passes.begin(); it != passes.end(); it++) {
		if ((vpass = dynamic_cast<VectorPass*> (*it)))
			fc.filterVectorPass(vpass);
	/*	else if ((rpass = dynamic_cast<RasterPass*> (*it)))
			fc.filterRasterPass(rpass);*/
	}
}

void Driver::process(LaserJob *job) {
	filter(job);
	stringstream ss;
	job->serializeTo(ss);
	cerr << ss.str().size();
	ofstream out("/tmp/job.dump");
	out << ss.rdbuf();
	out.close();
}
