/*
 * ArithmeticMeanFunction.cpp
 *
 *  Created on: 20 paź 2021
 *      Author: oramus
 */

#include "ArithmeticMeanFunction.h"

ArithmeticMeanFunction::ArithmeticMeanFunction(int dataPortionSize) :
		MagicFuntion(dataPortionSize) {
}

double ArithmeticMeanFunction::calc( double *dataPortion ) {
	double sum = 0;
	for ( int i = 0; i < dataPortionSize; i++ )
		sum += dataPortion[ i ];
	return sum / dataPortionSize;
}
