/*
 * Parameters.cpp
 *
 *  Created on: 18 gru 2021
 *      Author: oramus
 */

#include "ParametersAndResults.h"
#include"Alloc.h"

#include<iostream>
#include<fstream>
#include<math.h>

using namespace std;

void fill(int **table, int *idx, int value, int reps, int size) {
	int idx1, idx2;
	for (int r = 0; r < reps; r++) {
		idx1 = ((*idx) + r) / size;
		idx2 = ((*idx) + r) % size;
		table[idx1][idx2] = value;
	}
	*idx += reps;
}

ParametersAndResults* loadDataFromFile(string fname) {
	ParametersAndResults *param = new ParametersAndResults();
	ifstream report;
	report.open(fname);
	if (!report.good()) {
		param->size = -1;
	} else {
		report >> param->size;

		report >> param->generations;
		report >> param->perGenerationTime;

		report >> param->avgRepetitions;
		report >> param->perAvgTime;

		report >> param->maxRepetitions;
		report >> param->perMaxTime;

		report >> param->statRepetitions;
		report >> param->perStatTime;

		report >> param->globalTime;

		int value;
		int size2 = param->size * param->size;
		param->cellsLastState = tableAlloc(param->size);
		int idx = 0;
		do {
			report >> value;
			if (value == 0) {
				fill(param->cellsLastState, &idx, 0, 1, param->size);
			}
			if (value == 1) {
				fill(param->cellsLastState, &idx, 1, 1, param->size);
			}
			if (value > 1) {
				fill(param->cellsLastState, &idx, 0, value, param->size);
			}
		} while (idx != size2);

		param->numberOfNeighboursStatistics = new int[9];
		for (int i = 0; i < 9; i++) {
			report >> value;
			param->numberOfNeighboursStatistics[i] = value;
		}

		report >> param->avgNumerOfLiveNeighboursOfLiveCell;
		report >> param->maxSumOfNeighboursAge;
	}
	report.close();
	return param;
}

void saveSimulationDataInFile(ParametersAndResults *param, string fname) {
	ofstream report(fname);

	report << param->size << endl;

	report << param->generations << endl;
	report << param->perGenerationTime << endl;

	report << param->avgRepetitions << endl;
	report << param->perAvgTime << endl;

	report << param->maxRepetitions << endl;
	report << param->perMaxTime << endl;

	report << param->statRepetitions << endl;
	report << param->perStatTime << endl;

	report << param->globalTime << endl;

	int zeros = 0;
	for (int i = 0; i < param->size; i++)
		for (int j = 0; j < param->size; j++) {
			if (param->cellsLastState[i][j] == 0) {
				zeros++;
			} else {
				if (zeros == 0) {
					report << 1 << endl;
					continue;
				}
				if (zeros == 1) {
					report << 0 << endl;
					report << 1 << endl;
					zeros = 0;
					continue;
				}
				report << zeros << endl;
				report << 1 << endl;
				zeros = 0;
			}
		}
	report << zeros << endl;

	for (int i = 0; i < 9; i++)
		report << param->numberOfNeighboursStatistics[i] << endl;

	report << param->avgNumerOfLiveNeighboursOfLiveCell << endl;
	report << param->maxSumOfNeighboursAge << endl;
	report.close();
}

ParametersAndResults* createParameters() {
	struct ParametersAndResults *params = new ParametersAndResults();
	params->size = 4000;
	params->generations = 100;
	params->avgRepetitions = 4000;
	params->maxRepetitions = 300;
	params->statRepetitions = 300;
	return params;
}

bool compareResults(ParametersAndResults *actual, ParametersAndResults *expected) {
	if (fabs(
			actual->avgNumerOfLiveNeighboursOfLiveCell
					- expected->avgNumerOfLiveNeighboursOfLiveCell) > 0.0001) {
		cout << "Błąd w avgNumerOfLiveNeighboursOfLiveCell" << endl;
		cout << "Jest ..... " << actual->avgNumerOfLiveNeighboursOfLiveCell
				<< endl;
		cout << "Oczekiwano " << expected->avgNumerOfLiveNeighboursOfLiveCell
				<< endl;
		return false;
	}

	if (fabs(actual->maxSumOfNeighboursAge - expected->maxSumOfNeighboursAge)
			> 0.0001) {
		cout << "Błąd w maxSumOfNeighboursAge" << endl;
		cout << "Jest ..... " << actual->maxSumOfNeighboursAge << endl;
		cout << "Oczekiwano " << expected->maxSumOfNeighboursAge << endl;
		return false;
	}

	for (int i = 0; i < 9; i++) {
		if (actual->numberOfNeighboursStatistics[i]
				!= expected->numberOfNeighboursStatistics[i]) {
			cout << "Błąd w numberOfNeighboursStatistics" << endl;
			cout << "idx " << i << endl;
			cout << "Jest ..... " << actual->numberOfNeighboursStatistics[i]
					<< endl;
			cout << "Oczekiwano " << expected->numberOfNeighboursStatistics[i]
					<< endl;
			return false;
		}
	}

	for (int i = 0; i < expected->size; i++)
		for (int j = 0; j < expected->size; j++) {
			if (actual->cellsLastState[i][j]
					!= expected->cellsLastState[i][j]) {
				cout << "Bład w cellState " << endl;
				return false;
			}
		}

	return true;
}

bool testEfficiency() {
	return false;
}

