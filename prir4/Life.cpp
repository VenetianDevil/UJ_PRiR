/*
 * Life.cpp
 *
 *  Created on: 4 lis 2021
 *      Author: oramus
 */

#include "Life.h"
#include "Alloc.h"

Life::Life() {
}

Life::~Life(){

}

void Life::setRules(Rules *rules) {
	this->rules = rules;
}

void Life::setSize(int size) {
	this->size = size;
	this->age = tableAlloc(size);
	this->cells = tableAlloc(size);
	this->nextGeneration = tableAlloc(size);
	zeroTable(age);
	zeroTable(cells);
	zeroTable(nextGeneration);
}

void Life::zeroTable(int **tb) {
	for (int row = 0; row < size; row++)
		for (int col = 0; col < size; col++)
			tb[row][col] = 0;
}

void Life::setLiveCell(int col, int row) {
	cells[row][col] = 1;
}

int **Life::getCurrentState() {
	return cells;
}

// nie zrownoleglac
int Life::liveNeighbours(int row, int col) {
	return liveCell(row - 1, col - 1) + liveCell(row - 1, col)
			+ liveCell(row - 1, col + 1) + liveCell(row + 1, col - 1)
			+ liveCell(row + 1, col) + liveCell(row + 1, col + 1)
			+ liveCell(row, col - 1) + liveCell(row, col + 1);
}

// nie zrownoleglac
int Life::neighboursAgeSum(int row, int col) {
	return cellAge(row - 1, col - 1) + cellAge(row - 1, col)
			+ cellAge(row - 1, col + 1) + cellAge(row + 1, col - 1)
			+ cellAge(row + 1, col) + cellAge(row + 1, col + 1)
			+ cellAge(row, col - 1) + cellAge(row, col + 1);
}

int Life::liveCell(int row, int col) {
	if (row < 0)
		return 0;
	if (col < 0)
		return 0;
	if (row == size)
		return 0;
	if (col == size)
		return 0;
	return cells[row][col];
}

int Life::cellAge(int row, int col) {
	if (row < 0)
		return 0;
	if (col < 0)
		return 0;
	if (row == size)
		return 0;
	if (col == size)
		return 0;
	return age[row][col];
}

