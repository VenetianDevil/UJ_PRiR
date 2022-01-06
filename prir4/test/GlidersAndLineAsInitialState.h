/*
 * GlidersAndLineAsInitialState.h
 *
 *  Created on: 20 gru 2021
 *      Author: oramus
 */

#ifndef GLIDERSANDLINEASINITIALSTATE_H_
#define GLIDERSANDLINEASINITIALSTATE_H_

#include"Life.h"
#include"InitialStateGenerator.h"

class GlidersAndLineAsInitialState : public InitialStateGenerator {
private:
	void glider(Life *l, int col, int row);
	void lineH( Life *, int row );
public:
	GlidersAndLineAsInitialState();
	void generateInitialState( Life *life );
};

#endif /* GLIDERSANDLINEASINITIALSTATE_H_ */
