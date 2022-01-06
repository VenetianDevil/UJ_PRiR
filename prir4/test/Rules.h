/*
 * Rules.h
 *
 *  Created on: 4 lis 2021
 *      Author: oramus
 */

#ifndef RULES_H_
#define RULES_H_

class Rules {
public:
	Rules();
	virtual bool cellDies( int liveNeighbours, int cellAge, double rnd ) = 0;
	virtual bool cellBecomesLive( int liveNeighbours, int neighboursAgeSum, double rnd ) = 0;
};

#endif /* RULES_H_ */
