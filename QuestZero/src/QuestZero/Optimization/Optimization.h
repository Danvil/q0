/*
 * Optimization.h
 *
 *  Created on: Sep 10, 2010
 *      Author: david
 */

#ifndef QUESTZERO_OPTIMIZATION_H_
#define QUESTZERO_OPTIMIZATION_H_
//---------------------------------------------------------------------------
#include "QuestZero/Policies/TracePolicy/NoTracer.h"
#include "QuestZero/Policies/InitialStatesPolicy.h"
#include "QuestZero/Policies/TakePolicy.h"
//---------------------------------------------------------------------------

template<
	typename _State,
	template<typename,class,class,class> class Algorithm,
	template<typename> class SinglePicker = InitialStatesPolicy::RandomPicker,
	template<typename> class Take = TakePolicy::TakeBest,
	template<typename> class Tracer = NoTracer
>
struct Optimization
: public Algorithm<_State, InitialStatesPolicy::ManyPicker<_State, SinglePicker>, Take<_State>, Tracer<_State> >
{
	typedef _State State;

	template<class Space, class Function>
	static TSample<State> Optimize(const Space& space, PTR(Function) function) {
		Optimization<State, Algorithm, SinglePicker, Take, Tracer> x;
		return x.optimize(space, function);
	}
};

//---------------------------------------------------------------------------
#endif
