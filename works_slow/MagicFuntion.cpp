/*
 * MagicFuntion.cpp
 *
 *  Created on: 20 paź 2021
 *      Author: oramus
 */

#include "MagicFuntion.h"

MagicFuntion::MagicFuntion(int dataPortionSize ) : dataPortionSize( dataPortionSize ) {
}

MagicFuntion::~MagicFuntion() {
}

int MagicFuntion::getDataPortionSize() {
	return dataPortionSize;
}

