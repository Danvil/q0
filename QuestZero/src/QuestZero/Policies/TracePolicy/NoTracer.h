/*
 * NoTracer.h
 *
 *  Created on: Sep 6, 2010
 *      Author: david
 */

#ifndef QUESTZERO_TRACER_NOTRACER_H_
#define QUESTZERO_TRACER_NOTRACER_H_

#include "QuestZero/Policies/TracePolicy.h"
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

template<typename State, typename Score>
struct NoTracer
: public ITracePolicy<State, Score>
{
	void trace(const TSampleSet<State, Score>&) {}
};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
