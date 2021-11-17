#include"EnergyCalculator.h"
#include"Helper.h"
#include<math.h>

const double EnergyCalculator::DISTANCE_DECAY = 0.25;

double EnergyCalculator::contribution( double v1, double v2, double distance ) {
    return -cos( v1 - v2 ) * exp( - distance * DISTANCE_DECAY );
}

double EnergyCalculator::calc( double *data, int size, int row, int col ) {
    double v = Helper::getValue( data, size, row, col );
    return contribution( v, Helper::getValue( data, size, row + 1, col ), 1.0 ) +
        contribution( v, Helper::getValue( data, size, row - 1, col ), 1.0 ) +
        contribution( v, Helper::getValue( data, size, row, col + 1 ), 1.0 ) +
        contribution( v, Helper::getValue( data, size, row, col -1 ), 1.0 ) +
        contribution( v, Helper::getValue( data, size, row + 1, col + 1 ), 1.41 ) +
        contribution( v, Helper::getValue( data, size, row + 1, col - 1 ), 1.41 ) +
        contribution( v, Helper::getValue( data, size, row - 1, col + 1 ), 1.41 ) +
        contribution( v, Helper::getValue( data, size, row - 1, col - 1 ), 1.41 ) +
        contribution( v, Helper::getValue( data, size, row + 2, col ), 2.0 ) +
        contribution( v, Helper::getValue( data, size, row - 2, col ), 2.0 ) +
        contribution( v, Helper::getValue( data, size, row, col + 2 ), 2.0 ) +
        contribution( v, Helper::getValue( data, size, row, col - 2), 2.0 );
}
