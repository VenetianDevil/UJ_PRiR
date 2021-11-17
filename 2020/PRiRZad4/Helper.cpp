#include"Helper.h"

int Helper::toIdx( int size, int row, int col ) {
   return row * size + col;
}

double Helper::getValue( double *array, int size, int row, int col ) {
   return array[ toIdx( size, row, col )];
}

void Helper::setValue( double *array, int size, int row, int col, double newValue ) {
   array[ toIdx( size, row, col )] = newValue;
}

void Helper::updateValue( double *array, int size, int row, int col, double delta ) {
   array[ toIdx( size, row, col )] += delta;
}
