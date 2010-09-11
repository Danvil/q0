/*
 * Optimization.h
 *
 *  Created on: Sep 10, 2010
 *      Author: david
 */

#ifndef QUESTZERO_OPTIMIZATION_H_
#define QUESTZERO_OPTIMIZATION_H_
//---------------------------------------------------------------------------
#include "Tracer/NoTracer.h"
#include "StartingValues.h"
#include "TakePolicy.h"
//---------------------------------------------------------------------------

template<
	typename _State,
	template<typename,class,class,class> class Algorithm,
	template<typename> class SinglePicker = StartingStatePicker::RandomPicker,
	template<typename> class Take = TakePolicy::TakeBest,
	template<typename> class Tracer = NoTracer
>
struct Optimization
: public Algorithm<_State, StartingStatePicker::ManyPicker<_State, SinglePicker>, Take<_State>, Tracer<_State> >
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
