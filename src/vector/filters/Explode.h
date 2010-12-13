#ifndef SPLITINTERSECTIONSFILTER_H_
#define SPLITINTERSECTIONSFILTER_H_

#include <string>
#include <list>
#include <iostream>
#include "../Cut.h"

class Explode {
 public:
  Explode() {
  }
  virtual ~Explode();
  void filter(Cut *cut);
};

#endif /* SPLITINTERSECTIONSFILTER_H_ */
