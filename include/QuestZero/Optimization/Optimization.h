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
#include "QuestZero/Policies/InitializePolicy.h"
#include "QuestZero/Policies/ExitPolicy.h"
#include "QuestZero/Policies/TracePolicy.h"
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

template<
	typename State_,
	typename Score_,
	template<typename,typename,typename,typename> class Algorithm,
	typename InitializePolicy = InitializePolicy::ManyPicker<State_,InitializePolicy::RandomPicker>,
	typename ExitPolicy = ExitPolicy::FixedChecks<State_>
>
struct Optimization
: public Algorithm<State_, Score_, InitializePolicy, ExitPolicy>
{
	typedef State_ State;
	typedef Score_ Score;
	typedef TSample<State,Score> Sample;

	template<typename Space, typename Function, typename Visitor>
	Sample Minimize(const Space& space, const Function& function, Visitor vis=TracePolicy::None()) {
		return this->template Optimize(space, function, std::less<Score>(), vis);
	}

	template<typename Space, typename Function, typename Visitor>
	Sample Maximize(const Space& space, const Function& function, Visitor vis=TracePolicy::None()) {
		return this->template Optimize(space, function, std::greater<Score>(), vis);
	}
};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
