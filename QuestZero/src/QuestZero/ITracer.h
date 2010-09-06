#ifndef QUEST_ZERO_ITRACER_H_
#define QUEST_ZERO_ITRACER_H_

#include "SampleSet.h"
#include <Danvil/Ptr.h>
#include <boost/foreach.hpp>

template<typename State>
class ITracer
{
public:
	virtual void trace(int progress, int total, const TSampleSet<State>& samples) = 0;
};

#endif
