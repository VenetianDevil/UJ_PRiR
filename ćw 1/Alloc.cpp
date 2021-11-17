/*
 * Alloc.cpp
 *
 *  Created on: 20 paź 2021
 *      Author: oramus
 */

#include "Alloc.h"

double **tableAlloc( int size ) {
	double **result;
	result = new double* [ size ]; // size rows
	for ( int i = 0; i < size; i++ )
		result[ i ] = new double[ size ]; // size cols
	return result;
}



