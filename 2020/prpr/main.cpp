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

    MyMPI *mmpi = new MyMPI();
    mmpi->MPI_Init(&ac, &av);

    Simulation *s = new Simulation(mmpi);
    EnergyCalculator *ec = new EnergyCalculator();
    s->setEnergyCalculator(ec);
    s->setDataToChangeInSingleStep(64);

    int myRank;
    mmpi->MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    if (myRank == 0)
    {
        RandomNumberGenerator *rng = new RandomNumberGenerator();
 
        double *data = new double[size * size];
        prepareData(data, size, rng);

        MonteCarlo *mc = new MonteCarlo(rng);
        mc->setCoefficient(107.5);
        s->setInitialData(data, size);
        s->setRandomNumberGenerator(rng);
        s->setMonterCarlo(mc);
        s->setMaxChange(0.1);
    }
    s->init();
    s->calcInitialTotalEnergy();

    if (myRank == 0)
    {
        cout << "TotalEnergy = " << s->getTotalEnergy() << endl;
    }

    for (int i = 0; i < 1000; i++)
        s->singleStep();

    mmpi->MPI_Finalize();
}
