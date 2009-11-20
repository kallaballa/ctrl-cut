/*
 * FilterChain.h
 *
 *  Created on: 10.11.2009
 *      Author: amir
 */

#ifndef FILTERCHAIN_H_
#define FILTERCHAIN_H_

#include <list>
#include <string>
#include "Filter.h"

using std::list;
using std::string;

typedef list<RasterFilter*> RasterFilterList;
typedef list<VectorFilter*> VectorFilterList;

class FilterChain {
public:
	RasterFilterList rasterChain;
	VectorFilterList vectorChain;

	FilterChain();
	virtual ~FilterChain();
	bool evaluate();
	void filterRasterPass(RasterPass* pass);
	void filterVectorPass(VectorPass* pass);
};

#endif /* FILTERCHAIN_H_ */
