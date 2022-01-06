/*
 * Alloc.cpp
 */

#include "Alloc.h"

int **tableAlloc( int size ) {
	int **result;
	result = new int* [ size ]; // size rows
	for ( int i = 0; i < size; i++ )
		result[ i ] = new int[ size ]; // size cols
	return result;
}



