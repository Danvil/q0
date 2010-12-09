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
#include "QuestZero/Policies/TracePolicy/NoTracer.h"
#include "QuestZero/Policies/InitialStatesPolicy.h"
#include "QuestZero/Policies/TakePolicy.h"
#include "QuestZero/Policies/TargetPolicy.h"
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

template<
	typename _State,
	typename _Score,
	template<class,class,class,class,class,class> class Algorithm,
	class Target = TargetPolicy::ScoreTarget<double>,
	template<class> class SinglePicker = InitialStatesPolicy::RandomPicker,
	template<class,class> class Take = TakePolicy::TakeBest,
	template<class,class> class Tracer = NoTracer
>
struct Optimization
: public Algorithm<_State, _Score, Target, InitialStatesPolicy::ManyPicker<_State, SinglePicker>, Take<_State,_Score>, Tracer<_State,_Score> >
{
	typedef _State State;
	typedef _Score Score;

	template<class Space, class Function>
	static TSample<State,Score> Optimize(const Space& space, const Function& function) {
		Optimization<State, Score, Algorithm, Target, SinglePicker, Take, Tracer> x;
		return x.optimize(space, function);
	}
};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
