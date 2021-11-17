/*
 * SequentialDataProcessor.cpp
 *
 *  Created on: 20 paź 2021
 *      Author: oramus
 */

#include "SequentialDataProcessor.h"
#include "Alloc.h"

void SequentialDataProcessor::shareData() {
	nextData = tableAlloc(dataSize);
	for (int i = 0; i < dataSize; i++)
		for (int j = 0; j < dataSize; j++)
			nextData[i][j] = data[i][j];
}

void SequentialDataProcessor::singleExecution() {
	double *buffer = new double[dataPortionSize];
	for (int row = margin; row < dataSize - margin; row++)
		for (int col = margin; col < dataSize - margin; col++) {
			createDataPortion(row, col, buffer);
			nextData[row][col] = function->calc(buffer);

		}
	delete[] buffer;
	double **tmp = data;
	data = nextData;
	nextData = tmp;
}

void SequentialDataProcessor::createDataPortion(int row, int col,
		double *buffer) {
	int counter = 0;
	for (int i = row - margin; i <= row + margin; i++)
		for (int j = col - margin; j <= col + margin; j++)
			buffer[counter++] = data[i][j];
}
