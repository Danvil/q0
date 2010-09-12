/*
 * ProgressAndBestToConsoleTracer.h
 *
 *  Created on: Sep 6, 2010
 *      Author: david
 */

#ifndef QUESTZERO_TRACER_PROGRESSANDBESTTOCONSOLETRACER_H_
#define QUESTZERO_TRACER_PROGRESSANDBESTTOCONSOLETRACER_H_

#include "QuestZero/Policies/TracePolicy.h"
#include <iostream>

template<typename State>
struct ProgressAndBestToConsoleTracer
: public ITracePolicy<State>
{
	ProgressAndBestToConsoleTracer()
	: _count(1)
	{}

	void trace(const TSampleSet<State>& samples) {
		std::cout << "Optimization Progress (" << _count++ << "): best=" << samples.best() << std::endl;
	}

private:
	unsigned long _count;
};

#endif
