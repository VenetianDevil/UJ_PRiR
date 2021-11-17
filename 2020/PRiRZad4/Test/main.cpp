#include <iostream>
#include <iomanip>
#include <sys/time.h>
#include "EnergyCalculator.h"
#include "RandomNumberGenerator.h"
#include "Helper.h"
#include "MonteCarlo.h"

#define ____SERIAL

#include "SimulationS.h"
#include "Simulation.h"

using namespace std;

void prepareData(double *data, int size, RandomNumberGenerator *rng)
{
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
        {
            if ((i < 2) || (j < 2) || (i >= size - 2) ||
                (j >= size - 2))
            {
                Helper::setValue(data, size, i, j, 0);
            }
            else
            {
                Helper::setValue(data, size, i, j, 6.28 * rng->getDouble());
            }
        }
}

int toInt(char *arg)
{
    return atoi(arg);
}

double toDouble(char *arg)
{
    return atof(arg);
}

int main(int ac, char **av)
{
    if (ac != 5)
    {
        cout << "RUN: ./a.out size data2change steps calcNeighbours"
             << endl;
        return 0;
    }
    int size = toInt(av[1]);
    int data2change = toInt(av[2]);
    int steps = toInt(av[3]);
    int calcNeighbours = toInt(av[4]);

#ifdef SERIAL
    SimulationS *s = new SimulationS();
#else
    Simulation *s = new Simulation();
#endif

    EnergyCalculator *ec = new EnergyCalculator();
    s->setEnergyCalculator(ec);
    s->setDataToChangeInSingleStep(data2change);

    RandomNumberGenerator *rng = new RandomNumberGenerator();
 
    double *data = new double[size * size];
    prepareData(data, size, rng);

    MonteCarlo *mc = new MonteCarlo(rng);
    mc->setCoefficient(250.0);
    s->setInitialData(data, size);
    s->setRandomNumberGenerator(rng);
    s->setMonterCarlo(mc);
    s->setMaxChange(0.15);
    s->calcInitialTotalEnergy();

		struct timeval time_now{};
    gettimeofday(&time_now, NULL );
    time_t startMsec = (time_now.tv_sec * 1000) + (time_now.tv_usec / 1000);

    for (int i = 0; i < steps; i++) {
        s->singleStep();

        if ( calcNeighbours ) {
          cout << "<Similar neighbours> = " << s->calcAvgNumberOfSimilarNeighbours(4, 0.8 ) << endl;  
          cout << "Max neighbours       = " << s->getMaxNeighbours() << endl;        
          cout << "<Similar neighbours> = " << s->calcAvgNumberOfSimilarNeighbours(5, 0.8 ) << endl;  
          cout << "Max neighbours       = " << s->getMaxNeighbours() << endl;        
          cout << "<Similar neighbours> = " << s->calcAvgNumberOfSimilarNeighbours(4, 0.9 ) << endl;  
          cout << "Max neighbours       = " << s->getMaxNeighbours() << endl;        
          cout << "<Similar neighbours> = " << s->calcAvgNumberOfSimilarNeighbours(5, 0.9 ) << endl;  
          cout << "Max neighbours       = " << s->getMaxNeighbours() << endl;        
        } else {  
          cout << "TotalEnergy          = " << s->getTotalEnergy() << endl;
        }
    } 

    gettimeofday(&time_now, NULL );
    time_t stopMsec = (time_now.tv_sec * 1000) + (time_now.tv_usec / 1000);

		cout << "Czas pracy: " << ( stopMsec - startMsec ) << "msec." << endl;
}
