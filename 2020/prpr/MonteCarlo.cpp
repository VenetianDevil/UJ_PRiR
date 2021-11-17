#include"MonteCarlo.h"
#include<math.h>

MonteCarlo::MonteCarlo( RandomNumberGenerator *eng ) {
    this->rng = rng;
}

void MonteCarlo::setCoefficient( double newValue ) {
    this->coefficient = newValue;
}

bool MonteCarlo::accept( double oldEnergy, double newEnergy ) {
    return exp( coefficient * ( oldEnergy - newEnergy ) ) > rng->getDouble(); 
//    return oldEnergy > newEnergy;
}