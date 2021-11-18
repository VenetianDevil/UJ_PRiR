#include "MPIDataProcessor.h"
#include "Alloc.h"
#include "mpi.h"
#include <iostream>
#include <math.h>
#include <iomanip>

#define DATA_SIZE_TAG 1
#define NEXT_DATA_TAG 2
#define FLAT_DATA_TAG 3
#define RESULT_TAG 4
#define PREV_RIGHT_MARGIN_TAG 5
#define NEXT_LEFT_MARGIN_TAG 6
#define CASCADE_RIGHT_MARGIN_TAG 7
#define CASCADE_LEFT_MARGIN_TAG 8

using namespace std;

void MPIDataProcessor::shareData()
{
  processes; // liczba procesow
  MPI_Comm_size(MPI_COMM_WORLD, &processes);
  rank; // id procesu
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Status status;
  MPI_Barrier(MPI_COMM_WORLD);

  if (rank == 0)
  {
    int flatDataSize = dataSize * dataSize;
    flatData = new double[flatDataSize];
    for (int row = 0; row < dataSize; row++)
    {
      for (int col = 0; col < dataSize; col++)
      {
        flatData[(row * dataSize) + col] = data[row][col];
      }
    }

    for (int processRank = 1; processRank < processes; processRank++)
    {
      MPI_Send(&dataSize, 1, MPI_INT, processRank, DATA_SIZE_TAG, MPI_COMM_WORLD);
      MPI_Send(flatData, flatDataSize, MPI_DOUBLE, processRank, FLAT_DATA_TAG, MPI_COMM_WORLD);
    }
  }
  else
  {
    MPI_Recv(&dataSize, 1, MPI_INT, 0, DATA_SIZE_TAG, MPI_COMM_WORLD, &status);
    int flatDataSize = dataSize * dataSize;
    flatData = new double[flatDataSize];
    MPI_Recv(flatData, flatDataSize, MPI_DOUBLE, 0, FLAT_DATA_TAG, MPI_COMM_WORLD, &status);
  }

  calcFirstLastCol(rank, &firstCol, &lastCol);

}

void MPIDataProcessor::singleExecution()
{
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Status status;
  int flatDataSize = dataSize * dataSize;
  result = new double[flatDataSize];
  int marginDataSize = dataSize * margin;
  double *buffer = new double[dataPortionSize];

  int row;
  int col;
  int index;

  // uzupełnienie marginesów 1.00
  for (int i = 0; i < margin; i++)
  {
    for (int j = 0; j < dataSize; j++)
    {
      row = i;
      col = j;
      index = row * dataSize + col;
      result[index] = flatData[index];

      row = dataSize - 1 - i;
      col = j;
      index = row * dataSize + col;
      result[index] = flatData[index];

      row = j;
      col = i;
      index = row * dataSize + col;
      result[index] = flatData[index];

      row = j;
      col = dataSize - 1 - i;
      index = row * dataSize + col;
      result[index] = flatData[index];
    }
  }

  for (int row = margin; row < dataSize - margin; row++) //ok
    for (int col = firstCol; col <= lastCol; col++)
    {
      createDataPortion(row, col, buffer);
      result[(row * dataSize) + col] = function->calc(buffer);
    }
  delete[] buffer;

  MPI_Barrier(MPI_COMM_WORLD);

  if (rank > 0)
  {
    double *prevRigtMargin = new double[marginDataSize];
    getPrevRightMargin(prevRigtMargin, result);
    MPI_Send(prevRigtMargin, marginDataSize, MPI_DOUBLE, rank-1, PREV_RIGHT_MARGIN_TAG, MPI_COMM_WORLD);
    // cout << "rank: " << rank << " sent to " << rank - 1 << " its new right margin" << endl;

    double *leftMargin = new double[marginDataSize];
    MPI_Recv(leftMargin, marginDataSize, MPI_DOUBLE, rank-1, NEXT_LEFT_MARGIN_TAG, MPI_COMM_WORLD, &status);
    // cout << "rank: " << rank << " received from " << rank - 1 << " new left margin" << endl;
    setNewLeftMargin(leftMargin, result);
    
  }
  if (rank < processes - 1){ //poza ostatnim
    double *rigtMargin = new double[marginDataSize];
    MPI_Recv(rigtMargin, marginDataSize, MPI_DOUBLE, rank+1, PREV_RIGHT_MARGIN_TAG, MPI_COMM_WORLD, &status);
    // cout << "rank: " << rank << " received from " << rank+ 1 << " new right margin" << endl;
    setNewRightMargin(rigtMargin, result);

    // send next left margin
    double *nextLeftMargin = new double[marginDataSize];
    getNextLeftMargin(nextLeftMargin, result);
    MPI_Send(nextLeftMargin, marginDataSize, MPI_DOUBLE, rank+1, NEXT_LEFT_MARGIN_TAG, MPI_COMM_WORLD);
    // cout << "rank: " << rank << " sent to " << rank + 1 << " its new left margin" << endl;
  }

  flatData = result;
}

void MPIDataProcessor::getPrevRightMargin(double *prevRigtMargin, double *result){
  // cout << "rank " << rank << "send right margin col: " << firstCol << "-" << firstCol + margin - 1 << endl;
  for (int row = 0; row < dataSize; row++){
      for (int col = firstCol; col < firstCol + margin; col++)
      {
        prevRigtMargin[(row * margin) + (col - firstCol)] = result[(row * dataSize) + col];
      }
    }
}

void MPIDataProcessor::setNewRightMargin(double *rigtMargin, double *result){
  // cout << "rank " << rank << "recv right margin col: " << lastCol + 1 << "-" << lastCol + margin  << endl;
  for (int row = 0; row < dataSize; row++){
    for (int col = lastCol + 1; col <= lastCol + margin; col++)
    {
      result[(row * dataSize) + col] = rigtMargin[(row * margin) + (col - lastCol - 1)];
    }
  }
}

void MPIDataProcessor::getNextLeftMargin(double *nextLeftMargin, double *result){
  // cout << "rank " << rank << "send left margin col: " << lastCol - margin + 1 << "-" << lastCol << endl;
  for (int row = 0; row < dataSize; row++){
      for (int col = lastCol - margin + 1; col <= lastCol; col++)
      {
        nextLeftMargin[(row * margin) + (col - lastCol + margin - 1)] = result[(row * dataSize) + col];
      }
    }
}

void MPIDataProcessor::setNewLeftMargin(double *leftMargin, double *result){
  // cout << "rank " << rank << "recv left margin col: " << firstCol - margin << "-" << firstCol -1 << endl;
  for (int row = 0; row < dataSize; row++){
    for (int col = firstCol - margin; col < firstCol; col++)
    {
      result[(row * dataSize) + col] = leftMargin[(row * margin) + (col - firstCol + margin)];
    }
  }
}

void MPIDataProcessor::collectData()
{
  MPI_Barrier(MPI_COMM_WORLD);
  
  int flatDataSize = dataSize * dataSize;
  MPI_Status status;

  if (rank > 0){
    MPI_Send(flatData, flatDataSize, MPI_DOUBLE, 0, RESULT_TAG, MPI_COMM_WORLD);
    // cout << "rank: " << rank << " sent his result to process 0" << endl;
  }
  else
  {
    for (int processRank = 1; processRank < processes; processRank++)
    {
      double *partialResult = new double[flatDataSize];
      MPI_Recv(partialResult, flatDataSize, MPI_DOUBLE, processRank, RESULT_TAG, MPI_COMM_WORLD, &status);
      // cout << "rank: " << rank << " received partialResult from " << processRank << endl;

      int processFirstCol,
          processLastCol;
      calcFirstLastCol(processRank, &processFirstCol, &processLastCol);
      // cout << "rank: " << processRank << " processFirstCol " << processFirstCol << " processLastCol " << processLastCol  << endl;

      for (int row = margin; row < dataSize - margin; row++)
      {
        for (int col = processFirstCol; col <= processLastCol; col++)
        {
          int index = (row * dataSize) + col;
          flatData[index] = partialResult[index];
        }
      }
    }
    
    // cout << "end of putting all result togethen in new flatData" << endl;

    // przepisanie do data
    for (int row = 0; row < dataSize; row++)
    {
      for (int col = 0; col < dataSize; col++)
      {
        int index = (row * dataSize) + col;
        data[row][col] = flatData[index];
      }
    }
  }
 
}

void MPIDataProcessor::createDataPortion(int row, int col,
                                         double *buffer)
{
  int counter = 0;
  for (int i = row - margin; i <= row + margin; i++)
    for (int j = col - margin; j <= col + margin; j++)
    {
      buffer[counter++] = flatData[(i * dataSize) + j];
    }
}

void MPIDataProcessor::calcFirstLastCol(int processRank, int *firstCol, int *lastCol)
{
  // cout << "rank: " << processRank << "calcuklating cols" << endl;
  int defaultColCount = (dataSize - (margin * 2)) / processes;
  int leftoversCount = (dataSize - (margin * 2)) % processes;


  *firstCol = (defaultColCount * processRank) + margin + min(leftoversCount, processRank);
  *lastCol = *firstCol + defaultColCount - 1;
  if (processRank < leftoversCount)
  {
    // cout << "rank: " << processRank << " lastCol ++" << endl;
    *lastCol = *lastCol + 1;
  }
}
