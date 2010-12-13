#ifndef DEONIONFILTER_H_
#define DEONIONFILTER_H_

#include "../Cut.h"
#include "../Primitives.h"
#include <math.h>
#include <iostream>

class Deonion {
 public:
  Deonion()
    {
    }
  virtual ~Deonion();
  void filter(Cut *cut);
};

#endif /* DEONIONFILTER_H_ */
