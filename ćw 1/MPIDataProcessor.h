#ifndef MPIDATAPROCESSOR_H_
#define MPIDATAPROCESSOR_H_

#include "DataProcessor.h"

class MPIDataProcessor : public DataProcessor {
private:
	void createDataPortion( int row, int col, double *buffer );
protected:
	void singleExecution();
	void collectData() {}
	void shareData();
	int shareDataSize;
	int shareDataEnd;
	int rank;
	int processes;
public:
	double** getResult() {
		return data;
	}
};

#endif /* MPIDATAPROCESSOR_H_ */
