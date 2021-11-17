#include "SimulationS.h"
#include "Helper.h"
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <iomanip>

using namespace std;

SimulationS::SimulationS(MyMPI *_mmpi)
{
   mmpi = _mmpi;
}

void SimulationS::setRandomNumberGenerator(RandomNumberGenerator *randomNumberGenerator)
{
   this->rng = randomNumberGenerator;
}

void SimulationS::setEnergyCalculator(EnergyCalculator *energyCalculator)
{
   this->energyCalculator = energyCalculator;
}

void SimulationS::setMonterCarlo(MonteCarlo *mc)
{
   this->mc = mc;
}

void SimulationS::setMaxChange(double maxChange)
{
   this->maxChange = maxChange;
}

void SimulationS::setInitialData(double *data, int size)
{
   this->data = data;
   this->size = size;
   reducedSize = size - 4;
}

void SimulationS::init()
{
   // w wersji sekwencyjnej nie ma tu nic ciekawego
}

void SimulationS::calcInitialTotalEnergy()
{
   Etot = this->calcTotalEnergy();
}

double SimulationS::calcTotalEnergy()
{
   double Etot = 0.0;
   for (int row = 2; row < size - 2; row++)
      for (int col = 2; col < size - 2; col++)
         Etot += energyCalculator->calc(data, size, row, col);
   return Etot * 0.5;
}

double SimulationS::getTotalEnergy()
{
   return Etot;
}

void SimulationS::setDataToChangeInSingleStep(int dataToChange)
{
   this->dataToChange = dataToChange;
   rows = new int[dataToChange];
   cols = new int[dataToChange];
   delta = new double[dataToChange];
}

void SimulationS::generateDataChange()
{
   for (int i = 0; i < dataToChange; i++)
   {
      rows[i] = 2 + rng->getInt(reducedSize);
      cols[i] = 2 + rng->getInt(reducedSize);
      delta[i] = maxChange * (1.0 - 2.0 * rng->getDouble());
   }
}

void SimulationS::changeData()
{
   for (int i = 0; i < dataToChange; i++)
   {
      Helper::updateValue(data, size, rows[i], cols[i], delta[i]);
   }
}

void SimulationS::changeDataUndo()
{
   for (int i = 0; i < dataToChange; i++)
   {
      Helper::updateValue(data, size, rows[i], cols[i], -delta[i]);
   }
}

void SimulationS::singleStep()
{
   generateDataChange(); // wygenerowanie danych potrzebnych do zmiany stanu
   changeData(); // zmiana danych (stanu)

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