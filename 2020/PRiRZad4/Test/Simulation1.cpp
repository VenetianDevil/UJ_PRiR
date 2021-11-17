#include "Simulation.h"
#include "Helper.h"
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <iomanip>

using namespace std;

//#pragma omp threadprivate(drand_buf)

struct drand48_data Simulation::drand_buf {};

Simulation::Simulation()
{
	#pragma omp parallel
	{
		int seed = 1202107158 + omp_get_thread_num() * 1999;
		srand48_r (seed, &Simulation::drand_buf);
	}		
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

void Simulation::calcInitialTotalEnergy()
{
   Etot = this->calcTotalEnergy();
}

// do zrównoleglenia
double Simulation::calcTotalEnergy()
{
   double Etot = 0.0;
	
	
   #pragma omp parallel for schedule(static) reduction(+ : Etot)
   for (int row = 2; row < size - 2; row++)
      for (int col = 2; col < size - 2; col++)
      {
        //#pragma omp critical
        Etot += energyCalculator->calc(data, size, row, col);

      }
	

   return Etot * 0.5;
}

int Simulation::similarNeighbours(int col, int row, int delta, double limit)
{
   double middle = Helper::getValue(data, size, row, col);
   int neighbours = 0;

   //#pragma omp parallel for private(neighbours)
   for (int rowd = -delta; rowd <= delta; rowd++)
      for (int cold = -delta; cold <= delta; cold++)
      {
         if (cos(Helper::getValue(data, size, row + rowd, col + cold) - middle) > limit)
         {
            neighbours++;
         }
      }

   return neighbours - 1;
}

// do zrównoleglenia
double Simulation::calcAvgNumberOfSimilarNeighbours(int neighboursDistance, double limit)
{
   int sum = 0;
   int neighbours = 0;
   maxNeighbours = 0;
   int neighboursTmp;
	
   #pragma omp parallel for schedule(static) private(neighboursTmp) reduction(+ : sum, neighbours) //shared(maxNeighbours)
   for (int row = neighboursDistance; row < size - neighboursDistance; row++)
      for (int col = neighboursDistance; col < size - neighboursDistance; col++)
      {
         

         //#pragma omp critical
         {
         	neighboursTmp = similarNeighbours(col, row, neighboursDistance, limit);
         
         	sum += neighboursTmp;
         	
         	#pragma omp critical
         	if (neighboursTmp > maxNeighbours)
         	{
            	maxNeighbours = neighboursTmp;
         	}
	
         neighbours++;
         }
      }
	//#pragma omp barrier
	
   return (double)sum / (double)(neighbours * (neighboursDistance + 1) * 4 * neighboursDistance);
}

double Simulation::getTotalEnergy()
{
   return Etot;
}

int Simulation::getMaxNeighbours() {
   return maxNeighbours;
}

void Simulation::setDataToChangeInSingleStep(int dataToChange)
{
   this->dataToChange = dataToChange;
   rows = new int[dataToChange];
   cols = new int[dataToChange];
   delta = new double[dataToChange];
}

// do zrównoleglenia - konieczna wymiara generatora liczb losowych na drand48_r
void Simulation::generateDataChange()
{
	double tmp;
	int i = 0;
	int temp_int;
	//struct drand48_data drand_buf_local = Simulation::drand_buf;

   //#pragma omp parallel /*firstprivate(drand_buf)*/ private(tmp, i)//, drand_buf_local)
	{
   	//seed = 1202107158 + omp_get_thread_num() * 1999;
   	//srand48_r (time(NULL), &drand_buf_local);
   	//std::cout << "Test1\n";
   #pragma omp parallel for private(tmp, i)//schedule(dynamic)
   for (i = 0; i < dataToChange; i++)
   {
   	drand48_r(&Simulation::drand_buf, &tmp);
   	//std::cout << omp_get_thread_num() << "tak1: " << tmp << std::endl;
      rows[i] = 2 + (int)(tmp * reducedSize) ;//rng->getInt(reducedSize);
      
      drand48_r(&Simulation::drand_buf, &tmp);
      //std::cout << omp_get_thread_num() << "tak2: " << tmp << std::endl;
      cols[i] = 2 + (int)(tmp * reducedSize);//rng->getInt(reducedSize);
      drand48_r(&Simulation::drand_buf, &tmp);
      //std::cout << omp_get_thread_num() << "tak3: " << tmp << std::endl;
      delta[i] = maxChange * (1.0 - 2.0 * tmp);//rng->getDouble());

   }
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
   generateDataChange(); // wygenerowanie danych potrzebnych do zmiany stanu
   changeData();         // zmiana danych (stanu)

   // calcTotalEnergy
   double newEtot = calcTotalEnergy(); // wyliczenie nowej energii całkowitej

   // decyzja modulu MonteCarlo o akceptacji zmiany
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
