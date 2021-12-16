/*
 * Main.cpp
 *
 *  Created on: 4 lis 2021
 *      Author: oramus
 */

#include"Life.h"
#include"Rules.h"
#include"RandomRules.h"
#include"LifeSequentialImplementation.h"
#include"LifeParallelImplementation.h"
#include<iostream>
#include <unistd.h>
#include <ctime>
#include <omp.h>

using namespace std;

void showTable(int **cells, int size) {
	for (int row = 0; row < size; row++) {
		for (int col = 0; col < size; col++)
			cout << (cells[row][col] ? "X " : ". ");
		cout << endl;
	}
}

void showVector( int *tbl, int size ) {
	for ( int i = 0; i < size; i++ )
		cout << tbl[ i ] << ", ";
	cout << endl;
}

void glider(Life *l, int col, int row) {
	l->setLiveCell(col, row);
	l->setLiveCell(col + 1, row);
	l->setLiveCell(col + 2, row);
	l->setLiveCell(col, row + 1);
	l->setLiveCell(col + 1, row + 2);
}

int main(int argc, char **argv) {

	const int SIZE = 10000;

	// Life *l = new LifeSequentialImplementation(); // dla porownania
	Life *l = new LifeParallelImplementation();
	Rules *r = new RandomRules();

	l->setRules(r);
	l->setSize(SIZE);

	glider(l, 5, 5);
	glider(l, 10, 5);
	glider(l, 10, 10);
	glider(l, 5, 10);

	int *stat;
  int k=0;

  // clock_t start;
  // double duration;
  // start = std::clock(); // get current time

  // #pragma omp parallel
  // {
  //   #pragma omp single
  //   threadsNum = omp_get_num_threads();
  // }

	while (k<3) {
		l->oneStep();
    cout << "avgNumerOfLiveNeighboursOfLiveCell: " << l->avgNumerOfLiveNeighboursOfLiveCell() << endl;

    int max = l->maxSumOfNeighboursAge();      
    cout << "maxSumOfNeighboursAge: " << max << endl;

		stat = l->numberOfNeighboursStatistics();
	  for ( int i = 0; i < 9; i++ )
		  cout << "stat " << i << " = " << stat[ i ] << endl;
    
    // showTable(l->getCurrentState(), SIZE);
		// showVector( stat, 9 );
		delete[] stat;
		// usleep(250000);
    k++;
	}

  // cout << "time: " << ( clock() - start ) / (double) CLOCKS_PER_SEC << endl;

	return 0;
}

//  c++ -O -fopenmp *cpp && export OMP_NUM_THREADS=3 && /usr/bin/time ./a.out
// c++ -O *cpp && /usr/bin/time ./a.out