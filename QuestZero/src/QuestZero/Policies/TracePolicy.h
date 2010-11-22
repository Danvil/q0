#ifndef QUEST_ZERO_ITRACER_H_
#define QUEST_ZERO_ITRACER_H_

#include "QuestZero/Common/SampleSet.h"
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

template<typename State>
struct ITracePolicy
{
	virtual void trace(const TSampleSet<State>& samples) = 0;
};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
