/*
 * CuttingOptimizer.h
 *
 *  Created on: 26.09.2009
 *      Author: amir
 */

#ifndef CUTTINGOPTIMIZER_H_
#define CUTTINGOPTIMIZER_H_

extern "C" void optimize_vectors(char *vector_file, int x_center, int y_center);

class CuttingOptimizer {
public:
	CuttingOptimizer();
	virtual ~CuttingOptimizer();

};

#endif /* CUTTINGOPTIMIZER_H_ */
