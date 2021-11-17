#ifndef SIMULATIONS_H

#define SIMULATIONS_H

#include<iostream>
#include"MyMPI.h"
#include"EnergyCalculator.h"
#include"RandomNumberGenerator.h"
#include"MonteCarlo.h"

using namespace std;

class SimulationS {
	private:
		MyMPI *mmpi;
		MonteCarlo *mc;
		RandomNumberGenerator *rng;
		EnergyCalculator *energyCalculator;
		int size;  
		int reducedSize;
		int dataToChange;
		double maxChange;
		double *delta;
		int *rows;
		int *cols;
		double *data;
		double Etot;

		void changeData();
		void changeDataUndo();
		void generateDataChange();

	public:
		void init();
		double calcTotalEnergy();
		double getTotalEnergy();
		void calcInitialTotalEnergy();

		void setInitialData( double *data, int size );
		void setEnergyCalculator( EnergyCalculator *energyCalculator );
		void setRandomNumberGenerator( RandomNumberGenerator *randomNumberGenerator );
		void setMonterCarlo( MonteCarlo *mc );

		void setMaxChange( double maxChange );
		void setDataToChangeInSingleStep( int dataToChange );

		void singleStep();

		SimulationS( MyMPI *_mmpi );
};

#endif
