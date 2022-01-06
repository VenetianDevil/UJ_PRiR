/*
 * ConwayRules.h
 *
 *  Created on: Dec 18, 2021
 *      Author: uforamus
 */

#ifndef CONWAYRULES_H_
#define CONWAYRULES_H_

#include"Rules.h"

class ConwayRules : public Rules {
public:
	ConwayRules();
	virtual ~ConwayRules();
	bool cellDies( int liveNeighbours, int cellAge, double rnd );
	bool cellBecomesLive( int liveNeighbours, int neighboursAgeSum, double rnd );
};

#endif /* CONWAYRULES_H_ */
