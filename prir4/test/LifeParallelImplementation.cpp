/*
 * LifeParallelImplementation.cpp
 *
 *  Created on: 5 lis 2021
 *      Author: oramus
 */

#include "LifeParallelImplementation.h"
#include <stdlib.h>
#include <iostream>
#include <omp.h>
#include <vector>
#include<algorithm>

using namespace std;

LifeParallelImplementation::LifeParallelImplementation() {
  #pragma omp parallel
  {
    // cout << "konstrukcja" << omp_get_thread_num() << endl;
    #pragma omp single
    {
      th_num = omp_get_num_threads();
      // cout << "Threads : " << th_num << endl;

      buffers = new struct drand48_data[th_num];
      for ( int i = 0; i < th_num; i++ )
      {
        srand48_r(i, &buffers[i]);
      }
    }
  }
}

// do zrownoleglenia
void LifeParallelImplementation::oneStep() {
	int neighbours;
  double result;
  // struct drand48_data buffer;

  #pragma omp parallel default(none) private(neighbours, result) shared(nextGeneration, age, cells, rules, buffers)
  {
    // srand48_r((unsigned) time(NULL), &buffer);
    // srand48_r(omp_get_thread_num(), &buffer);
	  struct drand48_data buffer = buffers[omp_get_thread_num()];

    #pragma omp for collapse(2)
    for (int row = 0; row < size; row++){
      for (int col = 0; col < size; col++) {
        neighbours = liveNeighbours(row, col);

        if (cells[row][col]) {
          // komorka zyje
          drand48_r(&buffer, &result);
          if (rules->cellDies(neighbours, age[row][col], result)) {
          // if (rules->cellDies(neighbours, age[row][col], drand48())) {
            // smierc komorki
            nextGeneration[row][col] = 0;
            age[row][col] = 0;
          } else {
            // komorka zyje nadal, jej wiek rosnie
            nextGeneration[row][col] = 1;
            age[row][col]++;
          }
        } else {
          // komorka nie zyje
          drand48_r(&buffer, &result);
          if (rules->cellBecomesLive(neighbours, neighboursAgeSum(row, col), result)) {
              // neighboursAgeSum(row, col), drand48())) {
            // narodziny
            nextGeneration[row][col] = 1;
            age[row][col] = 0;
          } else {
            nextGeneration[ row ][ col ] = 0;
          }
        }
      }
    }
      
  }

	int **tmp = cells;
	cells = nextGeneration;
	nextGeneration = tmp;
}

// do zrownoleglenia
double LifeParallelImplementation::avgNumerOfLiveNeighboursOfLiveCell() {
	int sumOfNeighbours = 0;
	int counter = 0;
  int tempVal;
  
  #pragma omp parallel for collapse(2) reduction(+: sumOfNeighbours, counter)
	for ( int row = 1; row < size - 1; row ++ ){
		for ( int col = 1; col < size - 1; col++ ) {
			if ( cells[ row ][ col ] ) {
				sumOfNeighbours += liveNeighbours(row, col);
				counter++;
			}
		}
  }

	if ( counter == 0 ) return 0.0;
	return (double)sumOfNeighbours / (double)counter;
}

// do zrownoleglenia
int LifeParallelImplementation::maxSumOfNeighboursAge() {
	int sumOfNeighboursAge;
	int max = 0;

  #pragma omp parallel for collapse(2) schedule(static) private(sumOfNeighboursAge)
	for ( int row = 1; row < size - 1; row ++ ){
		for ( int col = 1; col < size - 1; col++ ) {
      
			sumOfNeighboursAge = neighboursAgeSum(row, col);

			if ( max < sumOfNeighboursAge ) {
        #pragma omp critical
        if ( max < sumOfNeighboursAge ) {
          max = sumOfNeighboursAge;
        }
      }
		}
  }

	return max;
}

// do zrownoleglenia
int* LifeParallelImplementation::numberOfNeighboursStatistics() {
	int *tbl = new int[ 9 ]; // od 0 do 8 sąsiadów włącznie

	for ( int i = 0; i < 9; i++ )
		tbl[ i ] = 0;

  #pragma omp parallel for collapse(2) reduction(+: tbl[:9])
	for ( int row = 1; row < size - 1; row ++ )
		for ( int col = 1; col < size - 1; col++ ) {
			tbl[ liveNeighbours(row, col) ]++;
		}

	return tbl;
}
