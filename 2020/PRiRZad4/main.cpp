#include <iostream>
#include <iomanip>
#include "Simulation.h"
#include "EnergyCalculator.h"
#include "RandomNumberGenerator.h"
#include "Helper.h"
#include "MonteCarlo.h"

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

int main(int ac, char **av)
{
    const int size = 1004;

    Simulation *s = new Simulation();
    EnergyCalculator *ec = new EnergyCalculator();
    s->setEnergyCalculator(ec);
    s->setDataToChangeInSingleStep(128);

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

    cout << "TotalEnergy = " << s->getTotalEnergy() << endl;

    for (int i = 0; i < 1000; i++) {
        s->singleStep();
        cout << "TotalEnergy          = " << s->getTotalEnergy() << endl;
        cout << "Test1\n";
        cout << "<Similar neighbours> = " << s->calcAvgNumberOfSimilarNeighbours(4, 0.8 ) << endl; 
        cout << "TEst2\n"; 
        cout << "Max neighbours       = " << s->getMaxNeighbours() << endl;
    }

}
