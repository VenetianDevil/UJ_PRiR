/*
 * GlidersAndLineAsInitialState.cpp
 *
 *  Created on: 20 gru 2021
 *      Author: oramus
 */

#include "GlidersAndLineAsInitialState.h"

GlidersAndLineAsInitialState::GlidersAndLineAsInitialState() {
}

void GlidersAndLineAsInitialState::glider(Life *l, int col, int row) {
	l->setLiveCell(col, row);
	l->setLiveCell(col + 1, row);
	l->setLiveCell(col + 2, row);
	l->setLiveCell(col, row + 1);
	l->setLiveCell(col + 1, row + 2);
}

void GlidersAndLineAsInitialState::lineH( Life *l, int row ) {
	for (int i = 0; i < size; i++)
		l->setLiveCell(i, row );
}

void GlidersAndLineAsInitialState::generateInitialState( Life *life ) {
	lineH( life, ( 1 * size ) / 4 );
	lineH( life, size / 2 );
	lineH( life, ( 3 * size ) / 4 );
	glider( life, ( 2 * size ) / 3,  ( 2 * size ) / 3 );
	glider( life, ( 2 * size ) / 3,  ( 3 * size ) / 4 );
	glider( life, ( 3 * size ) / 4,  ( 3 * size ) / 4 );
}
