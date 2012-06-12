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
	typename InitializePolicy = InitializePolicy::RandomPicker<State_>,
	typename ExitPolicy = ExitPolicy::FixedChecks<State_>
>
struct Optimization
: public Algorithm<State_, Score_, InitializePolicy, ExitPolicy>
{
	typedef Algorithm<State_, Score_, InitializePolicy, ExitPolicy> base_t;
	typedef State_ state_t;
	typedef Score_ score_t;
	typedef Sample<state_t,score_t> sample_t;

	/** Finds the optimum of a function over the given space
	 * @param space the space over which to optimize
	 * @param function the function to optimize
	 * @param compare compare functor used to compare scores
	 */
	template<typename Space, typename Function, typename Compare>
	sample_t Optimize(const Space& space, const Function& function, Compare compare) {
		return base_t::Optimize(space, function, compare, TracePolicy::None());
	}

	/** Finds the optimum of a function over the given space
	 * @param space the space over which to optimize
	 * @param function the function to optimize
	 * @param compare compare functor used to compare scores
	 * @param visitor used to trace evaluated particles
	 */
	template<class Space, class Function, typename Compare, typename Visitor>
	sample_t Optimize(const Space& space, const Function& function, Compare compare, Visitor visitor) {
		return base_t::Optimize(space, function, compare, visitor);
	}

	/** Finds the minimum of a function over the given space using std::less for score comparison
	 * @param space the space over which to optimize
	 * @param function the function to optimize
	 */
	template<typename Space, typename Function>
	sample_t Minimize(const Space& space, const Function& function) {
		return base_t::Optimize(space, function, std::less<score_t>(), TracePolicy::None());
	}

	/** Finds the maximum of a function over the given space using std::greater for score comparison
	 * @param space the space over which to optimize
	 * @param function the function to optimize
	 */
	template<typename Space, typename Function>
	sample_t Maximize(const Space& space, const Function& function) {
		return base_t::Optimize(space, function, std::greater<score_t>(), TracePolicy::None());
	}

	/** Finds the minimum of a function over the given space using std::less for score comparison
	 * @param space the space over which to optimize
	 * @param function the function to optimize
	 * @param visitor used to trace evaluated particles
	 */
	template<typename Space, typename Function, typename Visitor>
	sample_t Minimize(const Space& space, const Function& function, Visitor visitor) {
		return base_t::Optimize(space, function, std::less<score_t>(), visitor);
	}

	/** Finds the minimum of a function over the given space using std::less for score comparison
	 * @param space the space over which to optimize
	 * @param function the function to optimize
	 * @param visitor used to trace evaluated particles
	 */
	template<typename Space, typename Function, typename Visitor>
	sample_t Maximize(const Space& space, const Function& function, Visitor visitor) {
		return base_t::Optimize(space, function, std::greater<score_t>(), visitor);
	}

};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
