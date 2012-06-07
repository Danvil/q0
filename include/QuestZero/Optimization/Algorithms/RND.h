/*
 * RND.h
 *
 *  Created on: Sep 4, 2010
 *      Author: david
 */

#ifndef ALGORITHMS_RND_H
#define ALGORITHMS_RND_H
//---------------------------------------------------------------------------
#include "QuestZero/Common/Sample.h"
#include "QuestZero/Common/DefaultSampleList.h"
#include <string>
#include <vector>
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

/// <summary>
/// A optimization algorithm which tries to find the optimum of a function with stupid randomness
/// </summary>
template<typename State, typename Score,
	typename ExitPolicy,
	typename InitializePolicy
>
struct RND
: public ExitPolicy,
  public InitializePolicy
{
	typedef State state_t;
	typedef Score score_t;
	typedef TSample<state_t,score_t> sample_t;
	typedef TSampleSet<state_t,score_t> sample_set_t;

	RND() {
		particleCount = 100;
	}

	virtual ~RND() {}

	std::string name() const { return "RND"; }

	unsigned int particleCount;

	template<typename Space, typename Function, typename Compare, typename Visitor>
	sample_t Optimize(const Space& space, const Function& function, Compare cmp, Visitor vis) {
		sample_set_t open;
		this->PickInitial(open, space, particleCount);
		assert(num_samples(open) == particleCount);
		// evaluate initial samples
		compute_likelihood(open, function);
		// notify sample set observer
		vis.NotifySamples(open);
		// in every iteration add new particles, delete the worst particles
		// iterate until a given condition is fulfilled
		while(true) {
			// find best particle
			auto best_id = find_best_by_score(open, cmp);
			// check if it is good enough
			if(this->IsTargetReached(get_score(open, best_id), cmp)) {
				// return best sample
				return get_sample(open, best_id);
			}
			// randomly generate new states
			sample_set_t new_samples(space.template randomMany(particleCount));
			// evaluate new states
			compute_likelihood(new_samples, function);
			// add new samples
			add_samples(open, new_samples);
			// pick n best particles
			open = pick_best(open, particleCount, cmp);
			assert(num_samples(open) == particleCount);
			// notify sample set observer
			vis.NotifySamples(open);
		}
	}
};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
