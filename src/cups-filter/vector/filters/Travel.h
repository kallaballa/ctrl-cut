#ifndef TRAVEL_H_
#define TRAVEL_H_

#include "vector/CutModel.h"

class Travel {
public:
  Travel(){}
  virtual ~Travel() {}
  void filter(CutModel& cut);
};

#endif
