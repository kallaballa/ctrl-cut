#ifndef REDUCE_H_
#define REDUCE_H_

#include "vector/model/CutModel.h"

class Reduce
{
public:
  Reduce(float epsilon = 10) : epsilon(epsilon) {}
  virtual ~Reduce() {}

  void filter(CutModel &cut);

private:
  float epsilon;
};

#endif
