/*
 * LifeSequentialImplementation.h
 *
 *  Created on: 5 lis 2021
 *      Author: oramus
 */

#ifndef LIFESEQUENTIALIMPLEMENTATION_H_
#define LIFESEQUENTIALIMPLEMENTATION_H_

#include "Life.h"

class LifeSequentialImplementation: public Life {
public:
	LifeSequentialImplementation();
	double avgNumerOfLiveNeighboursOfLiveCell();
	int maxSumOfNeighboursAge();
	int* numberOfNeighboursStatistics();
	void oneStep();
};

#endif /* LIFESEQUENTIALIMPLEMENTATION_H_ */
