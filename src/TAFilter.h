/*
 * TAFilter.h
 *
 *  Created on: 10.11.2009
 *      Author: amir
 */

#ifndef TAFILTER_H_
#define TAFILTER_H_


#include "Filter.h"

class TAFilter: public RasterFilter {
 private:

 public:
  TAFilter() {
    this->provides.insert(TA_FILTER_ID);
    this->preFilters.insert(TP_FILTER_ID);
  }

  virtual ~TAFilter(){}
  virtual void filter(LaserPass *pass);
};

#endif /* TAFILTER_H_ */
