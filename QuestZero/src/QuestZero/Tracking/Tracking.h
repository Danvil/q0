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
#include "QuestZero/Tracking/Solution.h"
#include "QuestZero/Policies/TracePolicy.h"
#include "QuestZero/Policies/InitialStatesPolicy.h"
#include "QuestZero/Policies/TakePolicy.h"
#include "QuestZero/Policies/TargetPolicy.h"
///---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

template<
	typename Time_,
	typename State_,
	typename Score_,
	template<typename,typename,typename,class,class,class,class> class Algorithm,
	template<typename> class SinglePicker = InitialStatesPolicy::RandomPicker,
	template<typename,typename> class Take = TakePolicy::TakeMean,
	template<typename,typename> class NotifySamples = TracePolicy::Samples::None,
	template<typename,typename,typename> class NotifySolution = TracePolicy::Solution::None
>
struct Tracking
: public Algorithm<Time_, State_, Score_, InitialStatesPolicy::ManyPicker<State_, SinglePicker>, Take<State_,Score_>, NotifySamples<State_, Score_>, NotifySolution<Time_, State_, Score_> >
{
	typedef Time_ Time;

	typedef State_ State;

	typedef Score_ Score;

	//template<class Space, class Function>
	//static TSolution<Time, State, Score> Track(const TTimeRange<Time>& range, const Space& space, const Function& function) {
	//	Tracking<Time, State, Score, Algorithm, SinglePicker, Take, Tracer> x;
	//	return x.Track(range, space, function);
	//}
};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
