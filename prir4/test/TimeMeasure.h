/*
 * TimeMeasure.h
 *
 *  Created on: Dec 18, 2021
 *      Author: uforamus
 */

#ifndef TIMEMEASURE_H_
#define TIMEMEASURE_H_

#include <bits/stdc++.h>
#include <sys/time.h>

class TimeMeasure {
public:
	TimeMeasure();
	virtual ~TimeMeasure();

	void start();
	void stop();
	double getCpuUsage();
	double getRealTime();
	double getCoresUsage();
private:
	timespec cpuUsageStart, cpuUsageStop;
	timespec realTimeStart, realTimeStop;
	double timeTaken( timespec &start, timespec &stop );
};

#endif /* TIMEMEASURE_H_ */
