#ifndef DRIVER_H_
#define DRIVER_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include "LaserJob.h"

using std::ostream;

class Driver {
public:
  Driver();
  virtual ~Driver();
  void enableXML(bool on) { dumpxml = on; }
  void filter(LaserJob *job);
  void process(LaserJob *job, ostream& out);

private:
  bool dumpxml;
};

#endif /* DRIVER_H_ */
