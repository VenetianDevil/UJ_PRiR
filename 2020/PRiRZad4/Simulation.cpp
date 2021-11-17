#include "Simulation.h"
#include "Helper.h"
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <iomanip>

using namespace std;

//#pragma omp threadprivate(drand_buf)

Simulation::Simulation()
{
	std::cout << "kon strat\n";
	omp_set_nested(1);
	//drand_buf = new struct drand48_data[omp_get_num_threads()];
	std::cout << omp_get_num_threads() << std::endl;
	//for(int x = 0; x < omp_get_num_threads(); x++)
	//{
		//std::cout << x << std::endl;
		seed = 1202107158 + omp_get_thread_num() * 1999;
		srand48_r(seed, &drand_buf); 
	//} 
	
	std::cout << "Kon end\n";
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

	std::cout << "Start ec\n";
   #pragma omp parallel for reduction(+ : Etot)
   for (int row = 2; row < size - 2; row++)
      for (int col = 2; col < size - 2; col++)
      {
        //#pragma omp critical
        Etot += energyCalculator->calc(data, size, row, col);

      }
	std::cout << "End ec\n";

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
	std::cout << "sad s\n";
   #pragma omp parallel for schedule(static) private(neighboursTmp) reduction(+ : sum)
   for (int row = neighboursDistance; row < size - neighboursDistance; row++)
      for (int col = neighboursDistance; col < size - neighboursDistance; col++)
      {
         

         //#pragma omp critical
         {
         	neighboursTmp = similarNeighbours(col, row, neighboursDistance, limit);
         
         	sum += neighboursTmp;
         
         	if (neighboursTmp > maxNeighbours)
         	{
            	maxNeighbours = neighboursTmp;
         	}
	
         neighbours++;
         }
      }
	//#pragma omp barrier
	std::cout << "Sad e\n";
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
	std::cout << "Start\n";
	double tmp = 0;
	int i;
   // #pragma omp parallel for
   //int num_thread = omp_get_num_threads();
   //omp_set_nested(1);
   //#pragma omp parallel for private(i, drand_buf, tmp ) num_threads(1) shared(dataToChange)
   for (i = 0; i < dataToChange; i++)
   {
      rows[i] = 2 + rng->getInt(reducedSize);
      cols[i] = 2 + rng->getInt(reducedSize);
     // std::cout << "Test1\n";
      //std::cout << omp_get_thread_num() << std::endl;
      //drand48_r(&drand_buf, &tmp);
      // std::cout << "Test2\n";
      delta[i] = maxChange * (1.0 - 2.0 * rng->getDouble());
   }
   
   	std::cout << "End\n";
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
