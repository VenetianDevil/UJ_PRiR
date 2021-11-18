/*
 * DataProcessor.cpp
 *
 *  Created on: 20 paź 2021
 *      Author: oramus
 */

#include "DataProcessor.h"
#include <math.h>
#include <iostream>

using namespace std;

DataProcessor::DataProcessor() {
}

DataProcessor::~DataProcessor() {
}

int DataProcessor::round_near(double b) {
	if ((b - (int) b) < 0.5)
		return (int) b;
	else
		return (int) b + 1;
}

void DataProcessor::setMagicFunction(MagicFuntion *function) {
	this->function = function;
	setDataPortion( function->getDataPortionSize() );
}

void DataProcessor::setDataPortion(int dataPortionSize) {
	this->dataPortionSize = dataPortionSize;
	dataPortion = new double[dataPortionSize];
	setMargin();
}

void DataProcessor::setMargin() {
	margin = round_near(sqrt(dataPortionSize) - 1) / 2 ;
	cout << "Margin " << margin << endl;
}

void DataProcessor::setInitialData(double **data, int size) {
	this->data = data;
	this->dataSize = size;
}

void DataProcessor::execute(int repetitions) {
	shareData();
	for (int i = 0; i < repetitions; i++)
		singleExecution();
	collectData();
}
