#ifndef JOIN_H_
#define JOIN_H_

#include "util/LaserConfig.h"
#include "vector/CutModel.h"

class Join {
public:
  Join(){}
  virtual ~Join() {}
  void filter(CutModel& cut);
};

#endif
