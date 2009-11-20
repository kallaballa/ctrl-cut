/*
 * SplitIntersectionsFilter.h
 *
 *  Created on: 12.11.2009
 *      Author: amir
 */

#ifndef SPLITINTERSECTIONSFILTER_H_
#define SPLITINTERSECTIONSFILTER_H_

class SplitIntersectionsFilter : public VectorFilter {
public:
	SplitIntersectionsFilter() {
		this->provides.insert(SI_FILTER_ID);
	}
	virtual ~SplitIntersectionsFilter();
	void filter(VectorPass* vpass);
};

#endif /* SPLITINTERSECTIONSFILTER_H_ */
