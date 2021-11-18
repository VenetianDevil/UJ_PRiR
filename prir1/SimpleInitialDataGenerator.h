/*
 * SimpleInitialDataGenerator.h
 *
 *  Created on: 20 paź 2021
 *      Author: oramus
 */

#ifndef SIMPLEINITIALDATAGENERATOR_H_
#define SIMPLEINITIALDATAGENERATOR_H_

#include "InitialDataGenerator.h"

class SimpleInitialDataGenerator: public InitialDataGenerator {
	double marginValue;
	double bulkValue;
public:
	SimpleInitialDataGenerator( double marginValue, double bulkValue );
	void fillWithData( double **data, int dataSize, int margin );
};

#endif /* SIMPLEINITIALDATAGENERATOR_H_ */
