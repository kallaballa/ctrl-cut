#ifndef SPLITINTERSECTIONSFILTER_H_
#define SPLITINTERSECTIONSFILTER_H_

#include "Filter.h"

class SplitIntersectionsFilter : public VectorFilter {
 public:
  SplitIntersectionsFilter() {
    this->provides.insert(SI_FILTER_ID);
  }
  virtual ~SplitIntersectionsFilter();
  void filter(LaserPass *pass);
};

#endif /* SPLITINTERSECTIONSFILTER_H_ */
