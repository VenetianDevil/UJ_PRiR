/*
 * Life.h
 *
 *  Created on: 4 lis 2021
 *      Author: oramus
 */

#ifndef LIFE_H_
#define LIFE_H_

#include "Rules.h"

class Life {
protected:
	Rules *rules;
	int size;
	int **cells;
	int **nextGeneration;
	int **age;
	void zeroTable( int **tb );
	int liveNeighbours( int row, int col );
	int neighboursAgeSum( int row, int col );
	int liveCell( int row, int col );
	int cellAge( int row, int col );
public:
	Life();
	virtual ~Life();
	void setRules( Rules *rules );
	void setSize( int size );
	void setLiveCell( int col, int row );
	int **getCurrentState();

	virtual double avgNumerOfLiveNeighboursOfLiveCell() = 0;
	virtual int maxSumOfNeighboursAge() = 0;
	virtual int* numberOfNeighboursStatistics() = 0;
	virtual void oneStep() = 0;
};

#endif /* LIFE_H_ */
