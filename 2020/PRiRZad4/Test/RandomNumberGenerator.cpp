#include"RandomNumberGenerator.h"

double RandomNumberGenerator::getDouble() {
    return drand48();
}

int RandomNumberGenerator::getInt( int max ) {
    return (int)( drand48() * max );
}