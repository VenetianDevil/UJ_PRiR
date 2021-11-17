#ifndef SIMULATION_H

#define SIMULATION_H

#include<iostream>
#include"EnergyCalculator.h"
#include"RandomNumberGenerator.h"
#include"MonteCarlo.h"
#include <omp.h>

using namespace std;
//static struct drand48_data drand_buf;
//#pragma omp threadprivate(drand_buf)

class Simulation {
	private:
		MonteCarlo *mc;
		RandomNumberGenerator *rng;
		EnergyCalculator *energyCalculator;
		
		int seed;
		
		int size;  
		int reducedSize;
		int dataToChange;
		int maxNeighbours;
		double maxChange;
		double *delta;
		int *rows;
		int *cols;
		double *data;
		double Etot;

		void changeData();
		void changeDataUndo();
		void generateDataChange();

		int similarNeighbours( int col, int row, int delta, double limit );
	public:
		double calcTotalEnergy();
		double getTotalEnergy();
		void calcInitialTotalEnergy();
		double calcAvgNumberOfSimilarNeighbours( int neighboursDistance, double limit );

		void setInitialData( double *data, int size );
		void setEnergyCalculator( EnergyCalculator *energyCalculator );
		void setRandomNumberGenerator( RandomNumberGenerator *randomNumberGenerator );
		void setMonterCarlo( MonteCarlo *mc );

		void setMaxChange( double maxChange );
		void setDataToChangeInSingleStep( int dataToChange );

		void singleStep();
		int getMaxNeighbours();

		Simulation();
		
		static struct drand48_data drand_buf;
		#pragma omp threadprivate(drand_buf)
};

#endif
