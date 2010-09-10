/*
 * NoTracer.h
 *
 *  Created on: Sep 6, 2010
 *      Author: david
 */

#ifndef QUESTZERO_TRACER_NOTRACER_H_
#define QUESTZERO_TRACER_NOTRACER_H_

#include "../ITracer.h"

template<typename State>
struct NoTracer
: public ITracer<State>
{
	void trace(int progress, int total, const TSampleSet<State>& samples) {}
};

#endif
