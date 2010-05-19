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

void Driver::filter(LaserJob *job)
{
  FilterChain fc;

  if (!fc.evaluate()) {
    std::cerr << "Filter chain evaluation failed" << endl;
  }

  LaserPassList &passes = job->getPasses();

  for (LaserPassList::iterator it = passes.begin(); it != passes.end(); it++) {
    fc.filterPass(*it);
  }
}

void Driver::process(LaserJob *job)
{
  //  filter(job);
  stringstream ss;
  job->serializeTo(ss);
  std::cerr << ss.str().size();
  std::cout << ss.rdbuf();
}
