#ifndef DEONION_H_
#define DEONION_H_

#include "../Cut.h"
#include "../VTypes.h"
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
