#include "Simulation.h"
#include "Helper.h"
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <iomanip>

using namespace std;

Simulation::Simulation(MyMPI *_mmpi)
{
   mmpi = _mmpi;
}
Simulation::~Simulation()
{
	delete[] localData;
	delete[] displs;
	delete[] sendcounts;
}

void Simulation::setRandomNumberGenerator(RandomNumberGenerator *randomNumberGenerator)
{
   this->rng = randomNumberGenerator;
}

void Simulation::setEnergyCalculator(EnergyCalculator *energyCalculator)
{
   this->energyCalculator = energyCalculator;
}

void Simulation::setMonterCarlo(MonteCarlo *mc)
{
   this->mc = mc;
}

void Simulation::setMaxChange(double maxChange)
{
   this->maxChange = maxChange;
}

void Simulation::setInitialData(double *data, int size)
{
   this->data = data;
   this->size = size;
   reducedSize = size - 4;
}

void Simulation::init()
{
   // w wersji sekwencyjnej nie ma tu nic ciekawego
	mmpi->MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	mmpi->MPI_Comm_size(MPI_COMM_WORLD, &cores);
	if(cores>1)
	{
		if (rank==0)
		{
			for(int i=1; i<cores;i++)
				MPI_Send(&size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);	
		}
		else
		{
			MPI_Recv(&size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			reducedSize=size-4;
		}
	}
	this->minimumJob = (int)reducedSize / cores;
	remainedJob = reducedSize % cores;
	tmp=0;
	if(remainedJob)
	{
		localData = new double[(minimumJob+5)*size];
	}
	else
	{
		localData = new double[(minimumJob+4)*size];
	}
	displs=new int[cores]; //od ktorego miejsca wyslac elementy z data do localdata
	sendcounts= new int[cores]; //ile elementow wyslac
	for(int i=0; i<cores; i++)
	{
		sendcounts[i]=(minimumJob+4)*size;
	}
	
	for(int i=0; i<remainedJob; i++)
	{
		sendcounts[i]+=size;
	}
	for(int i=0; i<cores; i++)
	{
		displs[i]=tmp;
		tmp+=sendcounts[i]-(4*size);
	}
	tmp=1;
}

void Simulation::calcInitialTotalEnergy()
{
   Etot = this->calcTotalEnergy();
}

double Simulation::calcTotalEnergy()
{
   /*Etot = 0.0;
   for (int row = 2; row < size - 2; row++)
      for (int col = 2; col < size - 2; col++)
         Etot += energyCalculator->calc(data, size, row, col);
   return Etot * 0.5;
   */
	mmpi->MPI_Scatterv(data, sendcounts, displs, MPI_DOUBLE,
				localData, sendcounts[rank], MPI_DOUBLE,
				0, MPI_COMM_WORLD);
	double Etot = 0.0;
	double localResult=0.0;
	for (int row = 2; row < (sendcounts[rank]/size) - 2; row++)
		for (int col = 2; col < size - 2; col++)
			localResult += energyCalculator->calc(localData, size, row, col);
	mmpi->MPI_Reduce(&localResult, &Etot, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
   return Etot * 0.5;
}

double Simulation::getTotalEnergy()
{
   return Etot;
}

void Simulation::setDataToChangeInSingleStep(int dataToChange)
{
   this->dataToChange = dataToChange;
   rows = new int[dataToChange];
   cols = new int[dataToChange];
   delta = new double[dataToChange];
}

void Simulation::generateDataChange()
{
   for (int i = 0; i < dataToChange; i++)
   {
      rows[i] = 2 + rng->getInt(reducedSize);
      cols[i] = 2 + rng->getInt(reducedSize);
      delta[i] = maxChange * (1.0 - 2.0 * rng->getDouble());
   }
}

void Simulation::changeData()
{
   for (int i = 0; i < dataToChange; i++)
   {
      Helper::updateValue(data, size, rows[i], cols[i], delta[i]);
   }
}

void Simulation::changeDataUndo()
{
   for (int i = 0; i < dataToChange; i++)
   {
      Helper::updateValue(data, size, rows[i], cols[i], -delta[i]);
   }
}

void Simulation::singleStep()
{
	if(rank==0)
	{
	   generateDataChange(); // wygenerowanie danych potrzebnych do zmiany stanu
	   changeData(); // zmiana danych (stanu)
	}

   // calcTotalEnergy
   double newEtot = calcTotalEnergy(); // wyliczenie nowej energii całkowitej

   // decyzja modulu MonteCarlo o akceptacji zmiany
   if(rank==0)
   {
		if (mc->accept(Etot, newEtot))
	   {
		  cout << "Accepted Eold " << Etot << " newE " << newEtot << endl;
		  Etot = newEtot;
		  // zaakceptowano zmiane -> nowa wartosc energii calkowitej
	   }
	   else
	   {
		  changeDataUndo();
		  cout << "Not accepted Eold " << Etot << " newE " << newEtot << endl;
		  // zmiany nie zaakceptowano -> przywracany stary stan, energia bez zmiany
	   }
   }
}