/*
 * InitialDataGenerator.h
 *
 *  Created on: 20 paź 2021
 *      Author: oramus
 */

#ifndef INITIALDATAGENERATOR_H_
#define INITIALDATAGENERATOR_H_

class InitialDataGenerator {
public:
	virtual void fillWithData( double **data, int dataSize, int margin ) = 0;
};

#endif /* INITIALDATAGENERATOR_H_ */
