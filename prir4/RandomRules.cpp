/*
 * RandomRules.cpp
 *
 *  Created on: 5 lis 2021
 *      Author: oramus
 */

#include "RandomRules.h"

RandomRules::RandomRules() {
}


bool RandomRules::cellBecomesLive( int liveNeighbours, int neighboursAgeSum, double rnd ) {
	double p = 0.0;
	if ( liveNeighbours == 3 ) p = 0.9;
	if ( neighboursAgeSum > 100 ) p *= 0.5;
	return ( rnd < p );
}

bool RandomRules::cellDies( int liveNeighbours, int cellAge, double rnd ) {
	double p = 0.0;
	if ( liveNeighbours < 2 ) p = 0.9;
	if ( liveNeighbours > 3 ) p = 0.9;
	if ( cellAge > 32 ) p += 0.1;
	return ( rnd < p );
}
