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

// int FLAT_DATA_TAG = 3;
// int RESULT_TAG = 4;

using namespace std;

void showTable(double *table, int dataSize)
{
  // cout << "----------------------------------" << endl;
  for (int i = 0; i < dataSize; i++)
  {
    // cout << setw(3) << i << " -> ";
    for (int j = 0; j < dataSize; j++){
      // cout << " " << showpoint << setw(4) << setprecision(3) << table[i * dataSize + j];
    // cout << endl;
    }
  }
}

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
  // cout << "rank: " << rank << " firstCol " << firstCol << " lastCol " << lastCol  << endl;

}

void MPIDataProcessor::singleExecution()
{
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Status status;
  int flatDataSize = dataSize * dataSize;
  result = new double[flatDataSize];
  double *buffer = new double[dataPortionSize];

  int row;
  int col;
  int index;

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
    MPI_Send(result, flatDataSize, MPI_DOUBLE, 0, RESULT_TAG, MPI_COMM_WORLD);
    // cout << "rank: " << rank << " sent his result to process 0" << endl;
    MPI_Recv(flatData, flatDataSize, MPI_DOUBLE, 0, FLAT_DATA_TAG, MPI_COMM_WORLD, &status);
    // cout << "rank: " << rank << " received new flatData" << endl;
  }
  else
  {
    for (int processRank = 1; processRank < processes; processRank++)
    {
      double *partialResult = new double[flatDataSize];
      MPI_Recv(partialResult, flatDataSize, MPI_DOUBLE, processRank, RESULT_TAG, MPI_COMM_WORLD, &status);
      // cout << "rank: " << rank << " received result from " << processRank << endl;

      int processFirstCol,
          processLastCol;
      calcFirstLastCol(processRank, &processFirstCol, &processLastCol);
      // cout << "rank: " << processRank << " processFirstCol " << processFirstCol << " processLastCol " << processLastCol  << endl;

      for (int row = margin; row < dataSize - margin; row++)
      {
        for (int col = processFirstCol; col <= processLastCol; col++)
        {
          int index = (row * dataSize) + col;
          result[index] = partialResult[index];
        }
      }
    }

    // cout << "end of putting all result togethen in new flatData" << endl;

    flatData = result;
    for (int processRank = 1; processRank < processes; processRank++)
    {
      // cout << "new flatData sent to process Rank: " << processRank << endl;
      MPI_Send(flatData, flatDataSize, MPI_DOUBLE, processRank, FLAT_DATA_TAG, MPI_COMM_WORLD);
    }
  }

  // FLAT_DATA_TAG += 2;
  // RESULT_TAG += 2;
}

void MPIDataProcessor::collectData()
{
  if (rank == 0)
  {
    for (int row = 0; row < dataSize; row++)
    {
      for (int col = 0; col < dataSize; col++)
      {
        int index = (row * dataSize) + col;
        data[row][col] = result[index];
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
