#ifndef DEONION_H_
#define DEONION_H_

#include <math.h>
#include <iostream>

class Deonion {
public:
  Deonion()
  {
  }
  virtual ~Deonion();
  void filter(class Cut *cut);
};

#endif
