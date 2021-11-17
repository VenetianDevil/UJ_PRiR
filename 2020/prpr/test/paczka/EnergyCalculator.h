#ifndef ENERGY_CALCULATOR_H

#define ENERGY_CALCULATOR_H

#include<iostream>

using namespace std;

class EnergyCalculator {
    private:
      static const double DISTANCE_DECAY;
      double contribution( double v1, double v2, double distance );
	public:
      double calc( double *data, int size, int row, int col );
};

#endif
