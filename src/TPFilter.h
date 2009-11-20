/*
 * TPFilter.h
 *
 *  Created on: 10.11.2009
 *      Author: amir
 */

#ifndef TPFILTER_H_
#define TPFILTER_H_

#include "Filter.h"

class TPFilter: public RasterFilter {
private:

public:
	TPFilter() {
		this->provides.insert(TP_FILTER_ID);
		this->postFilters.insert(TA_FILTER_ID);
	}

	virtual ~TPFilter(){}
	virtual void filter(RasterPass* rpass);
};

#endif /* TPFILTER_H_ */
