/*
 * ArithmeticMeanFunction.h
 *
 *  Created on: 20 paź 2021
 *      Author: oramus
 */

#ifndef ARITHMETICMEANFUNCTION_H_
#define ARITHMETICMEANFUNCTION_H_

#include "MagicFuntion.h"

class ArithmeticMeanFunction : public MagicFuntion {
public:
	ArithmeticMeanFunction( int dataPortionSize );
	double calc( double *dataPortion );
};

#endif /* ARITHMETICMEANFUNCTION_H_ */
