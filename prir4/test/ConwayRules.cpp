/*
 * ConwayRules.cpp
 *
 *  Created on: Dec 18, 2021
 *      Author: uforamus
 */

#include "ConwayRules.h"

ConwayRules::ConwayRules() {
}

ConwayRules::~ConwayRules() {
}

bool ConwayRules::cellBecomesLive( int liveNeighbours, int neighboursAgeSum, double rnd ) {
	if ( liveNeighbours == 3 ) return true;
	return false;
}

bool ConwayRules::cellDies( int liveNeighbours, int cellAge, double rnd ) {
	if ( liveNeighbours < 2 ) return true;
	if ( liveNeighbours > 3 ) return true;
	return false;
}


