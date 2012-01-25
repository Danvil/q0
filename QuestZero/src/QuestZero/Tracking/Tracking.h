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
#include "QuestZero/Policies.hpp"
///---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

template<
	typename State_,
	typename Score_,
	template<typename,typename,class,class,class,class,class> class Algorithm,
	class TimeControl = Policies::TimeControlDelegate,
	template<typename> class SinglePicker = InitialStatesPolicy::RandomPicker,
	template<typename,typename> class Take = TakePolicy::TakeMean,
	template<typename,typename> class NotifySamples = TracePolicy::Samples::None,
	template<typename,typename> class TimestepResult = Policies::TimestepResultDummy
>
struct Tracking
: public Algorithm<
			State_, Score_,
			TimeControl,
			InitialStatesPolicy::ManyPicker<State_, SinglePicker>,
			Take<State_,Score_>,
			NotifySamples<State_, Score_>,
			TimestepResult<State_, Score_> >
{
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
