/*
 * Tracking.h
 *
 *  Created on: Sep 12, 2010
 *      Author: david
 */

#ifndef QUESTZERO_TRACKING_H_
#define QUESTZERO_TRACKING_H_
//---------------------------------------------------------------------------
#include "QuestZero/Common/SampleSet.h"
#include "QuestZero/Tracking/TimeRange.h"
#include "QuestZero/Policies/TracePolicy/NoTracer.h"
#include "QuestZero/Policies/InitialStatesPolicy.h"
#include "QuestZero/Policies/TakePolicy.h"
#include "QuestZero/Policies/TargetPolicy.h"
//---------------------------------------------------------------------------

template<
	typename Time_,
	typename State_,
	template<typename,typename,class,class,class> class Algorithm,
	template<typename> class SinglePicker = InitialStatesPolicy::RandomPicker,
	template<typename> class Take = TakePolicy::TakeMean,
	template<typename> class Tracer = NoTracer
>
struct Tracking
: public Algorithm<Time_, State_, InitialStatesPolicy::ManyPicker<State_, SinglePicker>, Take<State_>, Tracer<State_> >
{
	typedef Time_ Time;

	typedef State_ State;

	template<class Space, class Function>
	static TSample<State> Track(const TTimeRange<Time>& range, const Space& space, const Function& function) {
		Tracking<Time, State, Algorithm, SinglePicker, Take, Tracer> x;
		return x.track(range, space, function);
	}
};

//---------------------------------------------------------------------------
#endif
