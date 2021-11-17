#ifndef MONTE_CARLO_H_
#define MONTE_CARLO_H_

#include"RandomNumberGenerator.h"

class MonteCarlo {
    private:
        RandomNumberGenerator *rng;
        double coefficient;
    public:
        bool accept( double oldEnergy, double newEnergy );
        void setCoefficient( double newValue );
        MonteCarlo( RandomNumberGenerator *rng );
};

#endif
