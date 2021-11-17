#ifndef RANDOM_NUMBER_GENERATOR_H

#define RANDOM_NUMBER_GENERATOR_H

#include<stdlib.h>

class RandomNumberGenerator {
	public:
            RandomNumberGenerator( long int seedval ) {
                srand48( seedval );
            }
            RandomNumberGenerator() {
                srand48( 1024 );
            }
            double getDouble();
            int getInt( int max );
};

#endif
