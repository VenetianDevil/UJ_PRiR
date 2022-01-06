/*
 * TimeMeasure.cpp
 *
 *  Created on: Dec 18, 2021
 *      Author: uforamus
 */

#include "TimeMeasure.h"

TimeMeasure::TimeMeasure() {
	start();
}

TimeMeasure::~TimeMeasure() {
}

void TimeMeasure::start() {
    clock_gettime(CLOCK_MONOTONIC, &realTimeStart );
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &cpuUsageStart );
}

void TimeMeasure::stop() {
    clock_gettime(CLOCK_MONOTONIC, &realTimeStop );
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &cpuUsageStop );
}

double TimeMeasure::getCoresUsage() {
	if ( getRealTime() == 0 ) return 0.0;
	return getCpuUsage() / getRealTime();
}

double TimeMeasure::getCpuUsage() {
	return timeTaken(cpuUsageStart, cpuUsageStop);
}

double TimeMeasure::getRealTime() {
	return timeTaken(realTimeStart, realTimeStop);
}

double TimeMeasure::timeTaken( timespec &start, timespec &stop ) {
	return ( stop.tv_sec - start.tv_sec ) + ( stop.tv_nsec - start.tv_nsec ) / 1000000000.0;
}
