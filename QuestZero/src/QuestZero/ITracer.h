#ifndef QUEST_ZERO_ITRACER_H_
#define QUEST_ZERO_ITRACER_H_

#include "SampleSet.h"

template<typename State>
struct ITracer
{
	virtual void trace(int progress, int total, const TSampleSet<State>& samples) = 0;
};

#endif
