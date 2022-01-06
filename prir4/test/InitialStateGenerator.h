/*
 * InitialStateGenerator.h
 *
 *  Created on: 19 gru 2021
 *      Author: oramus
 */

#ifndef INITIALSTATEGENERATOR_H_
#define INITIALSTATEGENERATOR_H_

#include"Life.h"

class InitialStateGenerator {
protected:
	int size;
public:
	InitialStateGenerator();
	virtual void generateInitialState( Life *life ) = 0;
	void setSize( int size );
};

#endif /* INITIALSTATEGENERATOR_H_ */
