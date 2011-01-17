#ifndef QUEST_ZERO_ITRACER_H_
#define QUEST_ZERO_ITRACER_H_

#include "QuestZero/Common/SampleSet.h"
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

template<typename State, typename Score>
struct ITracePolicy
{
	virtual void trace(const TSampleSet<State,Score>& samples) = 0;
};

//---------------------------------------------------------------------------

template<typename State, typename Score>
struct ProgressToConsoleTracer
{
	ProgressToConsoleTracer()
	: _count(1)
	{}

	void trace(const TSampleSet<State,Score>& samples) {
		std::cout << "Optimization Progress (" << _count++ << ")" << std::endl;
	}

private:
	unsigned long _count;
};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
