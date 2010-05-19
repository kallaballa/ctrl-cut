/*
 * DeonionFilter.h
 *
 *  Created on: 15.11.2009
 *      Author: amir
 */

#ifndef DEONIONFILTER_H_
#define DEONIONFILTER_H_

#include "Filter.h"

class DeonionFilter: public VectorFilter {
 public:
  DeonionFilter()
    {
      this->provides.insert(DO_FILTER_ID);
      this->provides.insert(SER_PL_FILTER_ID);

      this->preFilters.insert(SI_FILTER_ID);
      this->preFilters.insert(FP_FILTER_ID);

      this->postFilters.insert(SER_OS_FILTER_ID);
    }
  virtual ~DeonionFilter();
  void filter(VectorPass* vpass);
};

#endif /* DEONIONFILTER_H_ */
