/*
 * CuttingOptimizer.h
 *
 *  Created on: 26.09.2009
 *      Author: amir
 */

#ifndef CUTTINGOPTIMIZER_H_
#define CUTTINGOPTIMIZER_H_
#include "LaserJob.h"

void optimizeVectorsPass(VectorPass *vpass);


class CuttingOptimizer {
public:

	CuttingOptimizer();
	virtual ~CuttingOptimizer();

};

#endif /* CUTTINGOPTIMIZER_H_ */
