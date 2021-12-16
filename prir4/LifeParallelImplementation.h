/*
 * LifeParallelImplementation.h
 *
 *  Created on: 5 lis 2021
 *      Author: oramus
 */

#ifndef LIFEPARALLELIMPLEMENTATION_H_
#define LIFEPARALLELIMPLEMENTATION_H_

#include"Life.h"

class LifeParallelImplementation : public Life {
public:
	LifeParallelImplementation();
	double avgNumerOfLiveNeighboursOfLiveCell();
	int maxSumOfNeighboursAge();
	int* numberOfNeighboursStatistics();
	void oneStep();
};

#endif /* LIFEPARALLELIMPLEMENTATION_H_ */
