/*
 * RandomRules.h
 *
 *  Created on: 5 lis 2021
 *      Author: oramus
 */

#ifndef RANDOMRULES_H_
#define RANDOMRULES_H_

#include"Rules.h"

class RandomRules : public Rules {
public:
	RandomRules();
	bool cellDies( int liveNeighbours, int cellAge, double rnd );
	bool cellBecomesLive( int liveNeighbours, int neighboursAgeSum, double rnd );
};

#endif /* RANDOMRULES_H_ */
