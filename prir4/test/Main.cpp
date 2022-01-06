/*
 * Main.cpp
 *
 *  Created on: 4 lis 2021
 *      Author: oramus
 */

#include"Life.h"
#include"Rules.h"
#include"ConwayRules.h"
#include"LifeSequentialDrand48R.h"
#include"LifeParallelImplementation.h"
#include"TimeMeasure.h"
#include"ParametersAndResults.h"
#include"InitialStateGenerator.h"
#include"GlidersAndLineAsInitialState.h"

#include<iostream>
#include<omp.h>

using namespace std;

void showTable(int **cells, int size) {
	if (size > 30)
		return;
	for (int row = 0; row < size; row++) {
		for (int col = 0; col < size; col++)
			cout << (cells[row][col] ? "X " : ". ");
		cout << endl;
	}
}

void showVector(int *tbl, int size) {
	for (int i = 0; i < size; i++)
		cout << tbl[i] << ", ";
	cout << endl;
}

void show(string info, int repetitions, double time, double cpu, double cores) {
	cout << "###########################" << endl;
	cout << info << endl;
	cout << "Repetitions ....... " << repetitions << endl;
	cout << "Total real time ... " << (time * repetitions) << " sec." << endl;
	cout << "Real time per call  " << time << " sec." << endl;
	cout << "CPU time per call.. " << cpu << " sec." << endl;
	cout << "Cores usage ....... " << (100.0 * cores) << "%" << endl;
}

void show(ParametersAndResults *params) {
	cout << "Size .............. " << params->size << endl;

	show("Statystyka pracy oneStep", params->generations,
			params->perGenerationTime, params->perGenerationCPU,
			params->perGenerationCores);

	show("Statystyka pracy avgNumerOfLiveNeighboursOfLiveCell",
			params->avgRepetitions, params->perAvgTime, params->perAvgCPU,
			params->perAvgCores);

	show("Statystyka pracy maxSumOfNeighboursAge", params->maxRepetitions,
			params->perMaxTime, params->perMaxCPU, params->perMaxCores);

	show("Statystyka pracy numberOfNeighboursStatistics",
			params->statRepetitions, params->perStatTime, params->perStatCPU,
			params->perStatCores);

	show("Globalny pomiar czasu pracy programu", 1, params->globalTime,
			params->globalCPU, params->globalCores);

	showTable(params->cellsLastState, params->size);
	showVector(params->numberOfNeighboursStatistics, 9);
	cout << "avgNumerOfLiveNeighboursOfLiveCell  "
			<< params->avgNumerOfLiveNeighboursOfLiveCell << endl;
	cout << "maxSumOfNeighboursAge ............. "
			<< params->maxSumOfNeighboursAge << endl;
}

void calculations(Life *l, Rules *rules, InitialStateGenerator *generator,
		ParametersAndResults *params) {

	l->setRules(rules);
	l->setSize(params->size);
	generator->generateInitialState(l);

	TimeMeasure *globalTM = new TimeMeasure();
	TimeMeasure *oneStepTM = new TimeMeasure();
	for (int step = 0; step < params->generations; step++) {
		l->oneStep();
	}
	oneStepTM->stop();

	params->perGenerationTime = oneStepTM->getRealTime() / params->generations;
	params->perGenerationCPU = oneStepTM->getCpuUsage() / params->generations;
	params->perGenerationCores = oneStepTM->getCoresUsage();
	params->cellsLastState = l->getCurrentState();

	double sum = 0.0;
	oneStepTM->start();
	for (int step = 0; step < params->avgRepetitions; step++)
		sum += l->avgNumerOfLiveNeighboursOfLiveCell();
	oneStepTM->stop();

	params->avgNumerOfLiveNeighboursOfLiveCell = sum;
	params->perAvgTime = oneStepTM->getRealTime() / params->avgRepetitions;
	params->perAvgCPU = oneStepTM->getCpuUsage() / params->avgRepetitions;
	params->perAvgCores = oneStepTM->getCoresUsage();

	sum = 0.0;
	oneStepTM->start();
	for (int step = 0; step < params->maxRepetitions; step++)
		sum += l->maxSumOfNeighboursAge();
	oneStepTM->stop();

	params->maxSumOfNeighboursAge = sum;
	params->perMaxTime = oneStepTM->getRealTime() / params->maxRepetitions;
	params->perMaxCPU = oneStepTM->getCpuUsage() / params->maxRepetitions;
	params->perMaxCores = oneStepTM->getCoresUsage();

	int *stat;
	oneStepTM->start();
	for (int step = 0; step < params->statRepetitions - 1; step++) {
		stat = l->numberOfNeighboursStatistics();
//		delete[] stat; // tu jest wyciek pamięci, ale niech tak zostanie
	}
	stat = l->numberOfNeighboursStatistics();
	oneStepTM->stop();
	globalTM->stop();

	params->perStatTime = oneStepTM->getRealTime() / params->statRepetitions;
	params->perStatCPU = oneStepTM->getCpuUsage() / params->statRepetitions;
	params->perStatCores = oneStepTM->getCoresUsage();
	params->numberOfNeighboursStatistics = stat;

	params->globalTime = globalTM->getRealTime();
	params->globalCPU = globalTM->getCpuUsage();
	params->globalCores = globalTM->getCoresUsage();

}

void cpuUsageTestFail(string name, double actual, double expected) {
	cout << "Oczekiwano lepszego użycia rdzeni w metodzie " << name << endl;
	cout << "Uzyskano ............. : " << actual << endl;
	cout << "Oczekiwano co najmniej : " << expected << endl;
}

bool testCPUusage(ParametersAndResults *params, double perMethodLimit,
		double globalLimit) {
	int threads = omp_get_max_threads();
	double globalCoresUsageExpected = globalLimit * threads;
	if (params->globalCores < globalCoresUsageExpected) {
		cout << "Oczekiwano lepszego globalnego użycia wątków" << endl;
		cout << "Globalnie uzyskano     : " << params->globalCores << endl;
		cout << "Oczekiwano co najmniej : " << globalCoresUsageExpected << endl;
		return false;
	}

	double perMethodCoresUsageExpected = perMethodLimit * threads;

	if (params->perGenerationCores < perMethodCoresUsageExpected) {
		cpuUsageTestFail("oneStep", params->perGenerationCores,
				perMethodCoresUsageExpected);
		return false;
	}

	if (params->perAvgCores < perMethodCoresUsageExpected) {
		cpuUsageTestFail("avgNumerOfLiveNeighboursOfLiveCell",
				params->perAvgCores, perMethodCoresUsageExpected);
		return false;
	}

	if (params->perMaxCores < perMethodCoresUsageExpected) {
		cpuUsageTestFail("maxSumOfNeighboursAge", params->perMaxCores,
				perMethodCoresUsageExpected);
		return false;
	}

	if (params->perStatCores < perMethodCoresUsageExpected) {
		cpuUsageTestFail("numberOfNeighboursStatistics", params->perStatCores,
				perMethodCoresUsageExpected);
		return false;
	}

	return true;
}

double efficiency(double seqTime, double parTime, int threads) {
	return seqTime / (parTime * threads);
}

void efficiencyTestFail(string name, double actual, double expected) {
	cout << "Oczekiwano wyższej efektywności obliczeń w metodzie " << name
			<< endl;
	cout << "Uzyskano ............. : " << actual << endl;
	cout << "Oczekiwano co najmniej : " << expected << endl;
}

bool efficiencyTest(ParametersAndResults *parallel,
		ParametersAndResults *sequential, double perMethodLimit,
		double globalLimit) {
	int threads = omp_get_max_threads();

	double globalEfficiency = efficiency(sequential->globalTime,
			parallel->globalTime, threads);

	if (globalEfficiency < globalLimit) {
		cout << "Efektywność programu jest zbyt mała" << endl;
		cout << "Uzyskano   : " << globalEfficiency * 100.0 << "%" << endl;
		cout << "Oczekiwano : " << globalLimit * 100.0 << "%" << endl;
		return false;
	} else {
	   cout << "Efektywność globalna .................................... = " << globalEfficiency << endl;
	}

	double methodEfficiency = efficiency(sequential->perGenerationTime,
			parallel->perGenerationTime, threads);

	if (methodEfficiency < perMethodLimit) {
		efficiencyTestFail("oneStep", methodEfficiency, perMethodLimit);
		return false;
	} else {
	   cout << "Efektywność dla metody oneStep .......................... = " << methodEfficiency << endl;
	}

	methodEfficiency = efficiency(sequential->perAvgTime, parallel->perAvgTime,
			threads);

	if (methodEfficiency < perMethodLimit) {
		efficiencyTestFail("avgNumerOfLiveNeighboursOfLiveCell",
				methodEfficiency, perMethodLimit);
		return false;
	} else {
	   cout << "Efektywność dla metody avgNumerOfLiveNeighboursOfLiveCell = " << methodEfficiency << endl;
	}

	methodEfficiency = efficiency(sequential->perMaxTime, parallel->perMaxTime,
			threads);

	if (methodEfficiency < perMethodLimit) {
		efficiencyTestFail("maxSumOfNeighboursAge", methodEfficiency,
				perMethodLimit);
		return false;
	} else {
	   cout << "Efektywność dla metody maxSumOfNeighboursAge ............ = " << methodEfficiency << endl;
	}

	methodEfficiency = efficiency(sequential->perStatTime,
			parallel->perStatTime, threads);

	if (methodEfficiency < perMethodLimit) {
		efficiencyTestFail("numberOfNeighboursStatistics", methodEfficiency,
				perMethodLimit);
		return false;
	} else {
	   cout << "Efektywność dla metody numberOfNeighboursStatistics ..... = " << methodEfficiency << endl;
	}

	return true;
}

int main(int argc, char **argv) {

	Rules *r = new ConwayRules();
	InitialStateGenerator *generator = new GlidersAndLineAsInitialState();

	ParametersAndResults *expected;
	expected = loadDataFromFile("data.txt");

	if (expected->size == -1) {
		cout << "Brak pliku z wynikiem sekwencyjnych obliczeń" << endl;
		cout << "Zostanie on teraz wygenerowany." << endl;
		cout << "Trochę cierpliwości..." << endl << endl;
		delete expected;
		expected = createParameters();
		generator->setSize(expected->size);
		calculations(new LifeSequentialDrand48R(), r, generator, expected);
		saveSimulationDataInFile(expected, "data.txt");
	} else {
		cout << "Oczekiwane wyniki pracy programu pobrane z pliku." << endl;
		cout << "Część parametrów może mieć wartość 0 i tak ma być!" << endl
				<< endl;
	}
	show(expected);

	ParametersAndResults *actual = createParameters();
	if (expected->size != actual->size) {
		cout << "Błąd danych wejściowych - plik zawiera inny rozmiar tablicy"
				<< endl;
		return 0;
	}

	generator->setSize(expected->size);
	cout << "Rozpoczyna się praca programu równoległego" << endl;
	calculations(new LifeParallelImplementation(), r, generator, actual);

	show(actual);

	cout << endl;

	cout << " Porównanie wyniku obliczeń: " << endl;
	bool result = compareResults(actual, expected);
	if (result) {
		cout << "  Porównanie wyników nie ujawniło błędu" << endl;
	} else {
		cout << "  Błąd w wyniku obliczeń" << endl;
		return 0;
	}

	cout << endl << "Teraz testy efektywności obliczeń współbieżnych: " << endl;

	// czy program używa wielu rdzeni
	result = testCPUusage(actual, 0.85, 0.9);

	if (result) {
		cout << "  Testy użycia rdzeni CPU zaliczone" << endl;
	} else {
		cout << "  Zbyt małe obciążenie CPU" << endl;
		return 0;
	}

	// przyspieszenie w stosunku do wersji sekwencyjnej

   cout << "------------------------------------------" << endl;

	result = efficiencyTest(actual, expected, 0.65, 0.8 );

	if (result) {
		cout << "  Testy efektywność obliczeń zaliczone" << endl << endl;
		cout << "............................." << endl;
		cout << "OK OK OK OK OK OK OK OK OK OK" << endl;
		cout << "OK OK OK OK OK OK OK OK OK OK" << endl;
		cout << "OK OK OK OK OK OK OK OK OK OK" << endl;
		cout << "OK OK OK OK OK OK OK OK OK OK" << endl;
		cout << "............................." << endl;
	} else {
		cout << "  Zbyt mała efektywność obliczeń !!!!!!!!!!" << endl;
		return 0;
	}

	return 0;
}

