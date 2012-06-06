/*
 * Optimization.h
 *
 *  Created on: Sep 10, 2010
 *      Author: david
 */

#ifndef QUESTZERO_OPTIMIZATION_H_
#define QUESTZERO_OPTIMIZATION_H_
//---------------------------------------------------------------------------
#include "QuestZero/Common/SampleSet.h"
#include "QuestZero/Policies/TracePolicy.h"
#include "QuestZero/Policies/InitialStatesPolicy.h"
#include "QuestZero/Policies/TakePolicy.h"
#include "QuestZero/Policies/TargetPolicy.h"
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

template<
	typename State_,
	typename Score_,
	template<class,class,class,class,class,class> class Algorithm,
	class Target,
	template<class> class Picker = InitialStatesPolicy::Fuser<InitialStatesPolicy::RandomPicker>::Result,
	template<class,class> class Take = TakePolicy::TakeBest,
	class NotifySamples = TracePolicy::Samples::BestToConsole<State_,Score_>
	//template<class,class> class NotifySamples = TracePolicy::Samples::BestToConsole
>
struct Optimization
: public Algorithm<State_, Score_, Target, Picker<State_>, Take<State_,Score_>, NotifySamples>
{
	typedef State_ State;
	typedef Score_ Score;
	typedef TSample<State,Score> Sample;

	template<class Space, class Function>
	Sample Minimize(const Space& space, const Function& function) {
		return this->template Optimize(space, function, std::less<Score>());
	}

	template<class Space, class Function>
	Sample Maximize(const Space& space, const Function& function) {
		return this->template Optimize(space, function, std::greater<Score>());
	}
};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
