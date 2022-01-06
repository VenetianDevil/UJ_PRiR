/*
 * Parameters.h
 *
 *  Created on: 18 gru 2021
 *      Author: oramus
 */

#ifndef PARAMETERSANDRESULTS_H_
#define PARAMETERSANDRESULTS_H_

#include<iostream>

using namespace std;

struct ParametersAndResults {
	int size;

	int generations;
	double perGenerationTime;
	double perGenerationCPU;
	double perGenerationCores;

	int avgRepetitions;
	double perAvgTime;
	double perAvgCPU;
	double perAvgCores;

	int maxRepetitions;
	double perMaxTime;
	double perMaxCPU;
	double perMaxCores;

	int statRepetitions;
	double perStatTime;
	double perStatCPU;
	double perStatCores;

	double globalTime;
	double globalCPU;
	double globalCores;

	int **cellsLastState;
	int *numberOfNeighboursStatistics;
	int avgNumerOfLiveNeighboursOfLiveCell;
	int maxSumOfNeighboursAge;

};

ParametersAndResults* createParameters();
ParametersAndResults* loadDataFromFile(string fname);
void saveSimulationDataInFile(ParametersAndResults *simInfo, string fname);
bool compareResults(ParametersAndResults *actual, ParametersAndResults *expected);


#endif /* PARAMETERSANDRESULTS_H_ */
