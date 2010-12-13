#ifndef FINDPOLYLINESFILTER_H_
#define FINDPOLYLINESFILTER_H_

#include <list>
#include <set>
#include "../Cut.h"
#include <iostream>

class FindPolylinesFilter {
 public:
  FindPolylinesFilter(){
  }

  virtual ~FindPolylinesFilter();
  void filter(Cut *cut);
};

#endif /* FINDPOLYLINESFILTER_H_ */
