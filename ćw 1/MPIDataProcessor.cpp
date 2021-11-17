#include "MPIDataProcessor.h"
#include "Alloc.h"
#include "mpi.h" 
#include <iostream>
#include <math.h>

using namespace std;

void MPIDataProcessor::shareData() {
  processes ; // liczba procesow
  MPI_Comm_size( MPI_COMM_WORLD, &processes );
  rank ; // id procesu
  MPI_Comm_rank( MPI_COMM_WORLD, &rank );
  MPI_Status status;
  if(rank == 0){
    for (int p = 0; p < processes; p++){
      int shareDataSize = dataSize / processes;
      int tag = 50;

      // ostatni proces dostaje reszte
      if(p + 1 == processes && dataSize % processes != 0){
        shareDataSize = dataSize % processes;
      }

      double **tempNextData = tableAlloc(dataSize);

      // cout << endl;
      // cout << "processes = " << processes << endl;
      for (int i = 0; i < dataSize; i++){
        tempNextData[i] = new double[ shareDataSize ];
        for (int j = 0; j < shareDataSize; j++){
          tempNextData[i][j] = data[i][p*shareDataSize + j];
        }
      }
      cout << "temp 0" << tempNextData[0][0] << endl;

      if(p != 0){
        cout << "temp data to be send to rank " << p << endl;
        MPI_Send(&dataSize, 1, MPI_INT, p, tag, MPI_COMM_WORLD);
        tag++;
        MPI_Send(tempNextData, dataSize*shareDataSize, MPI_DOUBLE, p, tag, MPI_COMM_WORLD);
        cout << " sent to " << p << endl;
      }
      else{
        nextData = tempNextData;
      }

      delete tempNextData;

    }
  }
  else{
    int tag = 50;
    cout << rank << " to be received " << endl;
    MPI_Recv(&dataSize, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);

    int shareDataSize = dataSize / processes;
    if(rank + 1 == processes && dataSize % processes != 0){
     // ostatni proces dostaje reszte
      shareDataSize = dataSize % processes;
    }
    tag++;

    nextData = tableAlloc(dataSize);
    MPI_Recv(nextData, dataSize*shareDataSize, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD, &status);
    cout << rank << " received " << nextData[0][0] << endl;
  }

  cout << "rank = " << rank << endl;
   for (int i = 0; i < dataSize; i++){
     cout << rank << " " << i << " --> " ;
    for (int j =0; j < shareDataSize; j++){
      cout << nextData[i][j] << " ";
    }
    cout << endl;
  }
  cout << "end rank = " << rank << endl;
}

void MPIDataProcessor::singleExecution() {
	double *buffer = new double[dataPortionSize];
  int colStart = 0;
  int colEnd = shareDataSize;
  if (rank == 0){
    colStart = margin;
  }
  if (rank + 1 == processes){
    colEnd = shareDataSize - margin;
  }
	for (int row = margin; row < dataSize - margin; row++) //ok
		for (int col =colStart; col < colEnd; col++) { // col = margin tylko dla procesu 0, 
			createDataPortion(row, col, buffer);
			nextData[row][col] = function->calc(buffer);

		}
	delete[] buffer;
	double **tmp = data;
	data = nextData;
	nextData = tmp;
}

void MPIDataProcessor::createDataPortion(int row, int col,
		double *buffer) {
	int counter = 0;
	for (int i = row - margin; i <= row + margin; i++)
		for (int j = col - margin; j <= col + margin; j++)
			buffer[counter++] = data[i][j];
}
