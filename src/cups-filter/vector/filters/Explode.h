#ifndef EXPLODE_H_
#define EXPLODE_H_

#include "vector/CutModel.h"

class Explode {
public:
  Explode() {}
  virtual ~Explode() {}
  void filter(CutModel& cut);
};

#endif
