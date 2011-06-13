#ifndef EXPLODE_H_
#define EXPLODE_H_

#include "util/LaserConfig.h"
#include "vector/CutModel.h"

class Explode {
public:
  Explode(){}
  virtual ~Explode() {}
  void filter(CutModel& cut);
};

#endif
