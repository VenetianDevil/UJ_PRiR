/*
 * LifeSequentialDrand48R.h
 *
 *  Created on: 20 gru 2021
 *      Author: oramus
 */

#ifndef LIFESEQUENTIALDRAND48R_H_
#define LIFESEQUENTIALDRAND48R_H_

#include "Life.h"
#include <stdlib.h>

class LifeSequentialDrand48R : public Life {
	private:
	struct drand48_data *drandBuffer;
public:
	LifeSequentialDrand48R();
	double avgNumerOfLiveNeighboursOfLiveCell();
	int maxSumOfNeighboursAge();
	int* numberOfNeighboursStatistics();
	void oneStep();
};

#endif /* LIFESEQUENTIALDRAND48R_H_ */
