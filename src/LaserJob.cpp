/*
 * LaserJob.cpp
 *
 *  Created on: 18.10.2009
 *      Author: amir
 */

#include "LaserJob.h"

LaserJob::LaserJob(laser_config *lconf, printer_config *pconf){
	this->lconf = lconf;
	this->pconf = pconf;
}

LaserJob::~LaserJob() {
	// TODO Auto-generated destructor stub
}

void LaserJob::addPass(LaserPass* pass)
{
	this->passes.push_back(pass);
}
