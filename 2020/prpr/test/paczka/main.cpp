#include <iostream>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include "Simulation.h"
#include "SimulationS.h"
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
                Helper::setValue(data, size, i, j, 3.14159265 * (1.0 - 2.0 * rng->getDouble()));
            }
        }
}

void getShowAndSaveTotalEnergy(SimulationS *s, int step, ofstream *report)
{
    double Etot = s->getTotalEnergy();
    cout << "TotalEnergy = " << Etot << endl;
    *report << setw(6) << step;
    *report << setiosflags(ios::showbase) << std::fixed << setw(12) << setprecision(5) << Etot
            << endl;
}

void getAndTestTotalEnergy(Simulation *s, int myRank, ifstream *report)
{
    if (myRank == 0)
    {
        int step;
        double EtotExpected;
        double Etot;
        *report >> step;
        *report >> EtotExpected;
        Etot = s->getTotalEnergy();
        if (fabs(Etot - EtotExpected) > 0.001)
        {
            cout << "Blad Etot! Jest " << Etot << " oczekiwano " << EtotExpected << endl;
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
    if (ac != 7)
    {
        cout << "RUN: mpirun -np procs ./a.out size data2change seed MC_coef maxChange steps"
             << endl;
        return 0;
    }
    int size = toInt(av[1]);
    int data2change = toInt(av[2]);
    int seed = toInt(av[3]);
    double MC_coef = toDouble(av[4]);
    double maxChange = toDouble(av[5]);
    int steps = toInt(av[6]);

    int myRank, processes;
    MyMPI *mmpi = new MyMPI();
    mmpi->MPI_Init(&ac, &av);
    mmpi->MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    mmpi->MPI_Comm_size(MPI_COMM_WORLD, &processes);

    ifstream report;
    int stopProgram = 0;

    if (myRank == 0)
    {
        cout << "size        " << size << endl;
        cout << "data2change " << data2change << endl;
        cout << "seed        " << seed << endl;
        cout << "MC coef     " << MC_coef << endl;
        cout << "maxChange   " << maxChange << endl;
        cout << "steps       " << steps << endl;
        cout << "processes   " << processes << endl;

        if (processes > 1)
        {
            report.open("serial.txt");
            if (!report.good())
            {
                report.close();
                cerr << "***************************************************************" << endl;
                cerr << "*  Brak pliku serial.txt z zapisem wykonania sekwencyjnego    *" << endl;
                cerr << "*  Aby go utworzyć należy uruchomić program z jednym procesem *" << endl;
                cerr << "***************************************************************" << endl;
                stopProgram = 1;
            }
            else
            {
                cout << "Znaleziono plik serial.txt - zostanie użyty do testu." << endl;
            }
        }
    }

    MPI_Bcast(&stopProgram, 1, MPI_INT, 0, MPI_COMM_WORLD); // rozesłanie informacji o wyłączeniu programu

    if (stopProgram)
    {
        mmpi->MPI_Finalize();
        return 0;
    }

    SimulationS *ss = new SimulationS(mmpi); // serial
    Simulation *sp = new Simulation(mmpi);   // parallel
    EnergyCalculator *ec = new EnergyCalculator();

    if (processes == 1)
    {
        ss->setEnergyCalculator(ec);
        ss->setDataToChangeInSingleStep(data2change);
    }
    else
    {
        sp->setEnergyCalculator(ec);
        sp->setDataToChangeInSingleStep(data2change);
    }

    if (myRank == 0)
    {
        RandomNumberGenerator *rng = new RandomNumberGenerator(seed);

        double *data = new double[size * size];
        prepareData(data, size, rng);

        MonteCarlo *mc = new MonteCarlo(rng);
        mc->setCoefficient(MC_coef);
        if (processes == 1)
        {
            ss->setInitialData(data, size);
            ss->setRandomNumberGenerator(rng);
            ss->setMonterCarlo(mc);
            ss->setMaxChange(maxChange);
        }
        else
        {
            sp->setInitialData(data, size);
            sp->setRandomNumberGenerator(rng);
            sp->setMonterCarlo(mc);
            sp->setMaxChange(maxChange);
        }
    }

    if (processes == 1)
    {
        ofstream report("serial.txt");
        ss->init();
        ss->calcInitialTotalEnergy();

        getShowAndSaveTotalEnergy(ss, 0, &report);

        double start = mmpi->MPI_Wtime();
        for (int i = 1; i <= steps; i++)
        {
            ss->singleStep();
            getShowAndSaveTotalEnergy(ss, i, &report);
        }
        double stop = mmpi->MPI_Wtime();
        cout << "Czas obliczen " << (stop - start) << endl;
        report << (stop - start) << endl;
        report.close();
    }
    else
    {
        ifstream report;
        if (myRank == 0)
        {
            report.open("serial.txt");
        }
        sp->init();
        sp->calcInitialTotalEnergy();

        getAndTestTotalEnergy(sp, myRank, &report);

        double start = mmpi->MPI_Wtime();
        for (int i = 0; i < steps; i++)
        {
            sp->singleStep();
            getAndTestTotalEnergy(sp, myRank, &report);
        }
        double stop = mmpi->MPI_Wtime();

        if (myRank == 0)
        {
            double obliczeniaSekwencyjne;
            report >> obliczeniaSekwencyjne;
            double obliczeniaRownolegle = stop - start;
            double przyspieszenie = obliczeniaSekwencyjne / obliczeniaRownolegle;
            double efektywnosc = przyspieszenie / processes;
            cout << "Czas obliczen równoległych  " << obliczeniaRownolegle << endl;
            cout << "Czas obliczen sekwencyjnych " << obliczeniaSekwencyjne << endl;
            cout << "Przyspieszenie              " << przyspieszenie << endl;
            cout << "Liczba procesow             " << processes << endl;
            cout << "Efektywnosc obliczen        " << efektywnosc << endl;
        }
    }
    mmpi->MPI_Finalize();
}
