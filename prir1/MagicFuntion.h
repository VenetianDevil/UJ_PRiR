/*
 * MagicFuntion.h
 *
 *  Created on: 20 paź 2021
 *      Author: oramus
 */

#ifndef MAGICFUNTION_H_
#define MAGICFUNTION_H_

class MagicFuntion {
protected:
	const int dataPortionSize;
public:
	MagicFuntion( int dataPortionSize );
	virtual ~MagicFuntion();
	int getDataPortionSize();
	virtual double calc( double *dataPortion ) = 0;
};

#endif /* MAGICFUNTION_H_ */
