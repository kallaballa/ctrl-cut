#ifndef DRIVER_H_
#define DRIVER_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include "Document.h"

class Document;

class Driver {
public:
  Driver() : dumpxml(false) {};
  virtual ~Driver(){};
  void enableXML(bool on) { dumpxml = on; }
  void filter(Document *job);

  void process(Document *job, std::ostream &stream);
private:
  bool dumpxml;
};

#endif /* DRIVER_H_ */
