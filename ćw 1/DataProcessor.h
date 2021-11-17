/*
 * DataProcessor.h
 *
 *  Created on: 20 paź 2021
 *      Author: oramus
 */

#ifndef DATAPROCESSOR_H_
#define DATAPROCESSOR_H_

#include"MagicFuntion.h"

class DataProcessor {
private:
	int round_near(double b);
protected:
	int margin;
	double **data;
	double **nextData;
	int dataSize;
	double *dataPortion;
	int dataPortionSize;

	MagicFuntion *function;

	void setMargin();
	virtual void singleExecution() = 0;
	virtual void shareData() = 0;
	virtual void collectData() = 0;
	virtual void setDataPortion( int dataPortion );
public:
	DataProcessor();
	virtual ~DataProcessor();
	virtual double** getResult() = 0;
	void setInitialData( double **data, int size );
	void execute( int repetitions );
	void setMagicFunction( MagicFuntion *function );
};

#endif /* DATAPROCESSOR_H_ */
