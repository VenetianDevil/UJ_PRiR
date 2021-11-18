#include <math.h> 
#include <iostream>
#include <iomanip>
#include <limits>
#include "mpi.h" 
#include "DataProcessor.h"
#include "MagicFuntion.h"
#include "ArithmeticMeanFunction.h"
#include "SimpleInitialDataGenerator.h"
#include "SequentialDataProcessor.h"
#include "Alloc.h"
#include "MPIDataProcessor.h"
#include <time.h>

using namespace std;

int calcDataPortion(int margin) {
	int dataPortion = margin * 2 + 1;
	dataPortion *= dataPortion;
	return dataPortion;
}

void showTable(double **table, int dataSize) {
	cout << "----------------------------------" << endl;
	for (int i = 0; i < dataSize; i++) {
		cout << setw(3) << i << " -> ";
		for (int j = 0; j < dataSize; j++)
			cout << " " << showpoint << setw(4) << setprecision(3)
					<< table[i][j];
		cout << endl;
	}
}

int main(int argc, char *argv[]) {

  time_t begin, end;
  time(&begin);
  
	MPI_Init(&argc, &argv);
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	const int MARGIN = 2;
	const int DATA_SIZE = 10000;
	const int REPETITIONS = 25;

	int dataPortion = calcDataPortion(MARGIN);
	MagicFuntion *mf = new ArithmeticMeanFunction(dataPortion);
	// DataProcessor *dp = new SequentialDataProcessor();
	DataProcessor *dp = new MPIDataProcessor();
	dp->setMagicFunction(mf);

	if (rank == 0) {
		double **initialData = tableAlloc(DATA_SIZE);
		InitialDataGenerator *generator = new SimpleInitialDataGenerator(1, 10);
		generator->fillWithData(initialData, DATA_SIZE, MARGIN);
		// showTable(initialData, DATA_SIZE);

		dp->setInitialData(initialData, DATA_SIZE);
	}

	dp->execute(REPETITIONS);

	if (rank == 0) {
		double **result = dp->getResult();
		// showTable(result, DATA_SIZE);
	}

	MPI_Finalize();

  time(&end);
  time_t elapsed = end - begin;
  cout << "Time measured: " << elapsed << endl;

	return 0;
}

