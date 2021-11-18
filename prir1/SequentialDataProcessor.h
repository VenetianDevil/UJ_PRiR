/*
 * SequentialDataProcessor.h
 *
 *  Created on: 20 paź 2021
 *      Author: oramus
 */

#ifndef SEQUENTIALDATAPROCESSOR_H_
#define SEQUENTIALDATAPROCESSOR_H_

#include "DataProcessor.h"

class SequentialDataProcessor : public DataProcessor {
private:
	void createDataPortion( int row, int col, double *buffer );
protected:
	void singleExecution();
	void collectData() {}
	void shareData();
public:
	double** getResult() {
		return data;
	}
};

#endif /* SEQUENTIALDATAPROCESSOR_H_ */
