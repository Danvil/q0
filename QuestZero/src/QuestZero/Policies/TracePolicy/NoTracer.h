/*
 * NoTracer.h
 *
 *  Created on: Sep 6, 2010
 *      Author: david
 */

#ifndef QUESTZERO_TRACER_NOTRACER_H_
#define QUESTZERO_TRACER_NOTRACER_H_

#include "QuestZero/Policies/TracePolicy.h"

template<typename State>
struct NoTracer
: public ITracePolicy<State>
{
	void trace(const TSampleSet<State>&) {}
};

#endif
