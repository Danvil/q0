/*
 * ProgressAndBestToConsoleTracer.h
 *
 *  Created on: Sep 6, 2010
 *      Author: david
 */

#ifndef QUESTZERO_TRACER_PROGRESSANDBESTTOCONSOLETRACER_H_
#define QUESTZERO_TRACER_PROGRESSANDBESTTOCONSOLETRACER_H_

#include "../ITracer.h"
#include <iostream>

template<typename State>
class ProgressAndBestToConsoleTracer
: public ITracer<State>
{
public:
	void trace(int progress, int total, const TSampleSet<State>& samples) {
		std::cout << "Optimization Progress: " << progress << "/" << total << ", best=" << samples.best() << std::endl;
	}
};

#endif
