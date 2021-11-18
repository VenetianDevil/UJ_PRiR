#ifndef MPIDATAPROCESSOR_H_
#define MPIDATAPROCESSOR_H_

#include "DataProcessor.h"

class MPIDataProcessor : public DataProcessor {
private:
	void createDataPortion( int row, int col, double *buffer );
	void calcFirstLastCol( int processRank, int *firstCol, int *lastCol );
  void getPrevRightMargin(double *prevRigtMargin, double *result);
  void setNewRightMargin(double *rigtMargin, double *result);
  void getNextLeftMargin(double *nextLeftMargin, double *result);
  void setNewLeftMargin(double *leftMargin, double *result);
protected:
	void singleExecution();
	void collectData();
	void shareData();
	int shareDataSize;
	int shareDataEnd;
	int rank;
	int processes;
	int firstCol;
	int lastCol;
	double *flatData;
	double *result;
public:
	double** getResult() {
		return data;
	}
};

#endif /* MPIDATAPROCESSOR_H_ */
