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
	class Target = TargetPolicy::ScoreTargetWithMaxChecks<Score_>,
	template<class> class SinglePicker = InitialStatesPolicy::RandomPicker,
	template<class,class> class Take = TakePolicy::TakeBest,
	template<class,class> class NotifySamples = TracePolicy::Samples::BestToConsole
>
struct Optimization
: public Algorithm<State_, Score_, Target, InitialStatesPolicy::ManyPicker<State_, SinglePicker>, Take<State_,Score_>, NotifySamples<State_,Score_> >
{
	typedef State_ State;
	typedef Score_ Score;

//	template<class Space, class Function>
//	static TSample<State,Score> Optimize(const Space& space, const Function& function) {
//		Optimization<State, Score, Algorithm, Target, SinglePicker, Take, Tracer> x;
//		return x.optimize(space, function);
//	}
};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
