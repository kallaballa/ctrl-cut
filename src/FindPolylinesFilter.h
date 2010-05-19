#ifndef FINDPOLYLINESFILTER_H_
#define FINDPOLYLINESFILTER_H_

#include "Filter.h"

class FindPolylinesFilter: public VectorFilter {
 public:
  FindPolylinesFilter(){
    this->provides.insert(FP_FILTER_ID);
    this->preFilters.insert(SI_FILTER_ID);
    this->postFilters.insert(SER_PL_FILTER_ID);
  }

  virtual ~FindPolylinesFilter();
  void filter(LaserPass *pass);
};

#endif /* FINDPOLYLINESFILTER_H_ */
