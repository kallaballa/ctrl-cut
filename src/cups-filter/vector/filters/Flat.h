#ifndef SORT_H_
#define SORT_H_

#include <math.h>
#include <iostream>
#include "vector/Polyline.h"

class Flat {
public:
  Flat()
  {
  }

  virtual ~Flat();
  void filter(class Cut *cut);
};

#endif
