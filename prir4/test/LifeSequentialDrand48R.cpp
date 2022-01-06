/*
 * LifeSequentialDrand48R.cpp
 *
 *  Created on: 20 gru 2021
 *      Author: oramus
 */

#include "LifeSequentialDrand48R.h"
#include <stdlib.h>

LifeSequentialDrand48R::LifeSequentialDrand48R() {
	drandBuffer = new struct drand48_data();
	srand48_r( 1234, drandBuffer);
}

void LifeSequentialDrand48R::oneStep() {
	int neighbours;
	double rnd;
	for (int row = 0; row < size; row++)
		for (int col = 0; col < size; col++) {
			neighbours = liveNeighbours(row, col);

			if (cells[row][col]) {
				// komorka zyje
				drand48_r(drandBuffer, &rnd);
				if (rules->cellDies(neighbours, age[row][col], rnd)) {
					// smierc komorki
					nextGeneration[row][col] = 0;
					age[row][col] = 0;
				} else {
					// komorka zyje nadal, jej wiek rosnie
					nextGeneration[row][col] = 1;
					age[row][col]++;
				}
			} else {
				// komorka nie zyje
				drand48_r(drandBuffer, &rnd);
				if (rules->cellBecomesLive(neighbours,
						neighboursAgeSum(row, col), rnd)) {
					// narodziny
					nextGeneration[row][col] = 1;
					age[row][col] = 0;
				} else {
					nextGeneration[row][col] = 0;
				}
			}
		}
	int **tmp = cells;
	cells = nextGeneration;
	nextGeneration = tmp;
}

double LifeSequentialDrand48R::avgNumerOfLiveNeighboursOfLiveCell() {
	int sumOfNeighbours = 0;
	int counter = 0;
	for (int row = 1; row < size - 1; row++)
		for (int col = 1; col < size - 1; col++) {
			if (cells[row][col]) {
				sumOfNeighbours += liveNeighbours(row, col);
				counter++;
			}
		}
	if (counter == 0)
		return 0.0;
	return (double) sumOfNeighbours / (double) counter;
}

int LifeSequentialDrand48R::maxSumOfNeighboursAge() {
	int sumOfNeighboursAge;
	int max = 0;

	for (int row = 1; row < size - 1; row++)
		for (int col = 1; col < size - 1; col++) {
			sumOfNeighboursAge = neighboursAgeSum(row, col);

			if (max < sumOfNeighboursAge) {
				max = sumOfNeighboursAge;
			}
		}

	return max;
}

int* LifeSequentialDrand48R::numberOfNeighboursStatistics() {
	int *tbl = new int[9]; // od 0 do 8 sąsiadów włącznie
	for (int i = 0; i < 9; i++)
		tbl[i] = 0;

	for (int row = 1; row < size - 1; row++)
		for (int col = 1; col < size - 1; col++) {
			tbl[liveNeighbours(row, col)]++;
		}

	return tbl;
}

