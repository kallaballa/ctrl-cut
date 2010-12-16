#ifndef EXPLODE_H_
#define EXPLODE_H_

#include <string>
#include <list>
#include <iostream>
#include <boost/format.hpp>
#include "../Cut.h"

class Explode {
 public:
  Explode() {
  }
  virtual ~Explode();
  void filter(Cut *cut);
};

#endif /* SPLITINTERSECTIONSFILTER_H_ */
