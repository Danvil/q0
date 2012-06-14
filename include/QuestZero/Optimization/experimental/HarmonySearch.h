/*
 * HarmonySearch.h
 *
 *  Created on: Jun 12, 2012
 *      Author: david
 */

#ifndef HARMONYSEARCH_H_
#define HARMONYSEARCH_H_

#include "QuestZero/Common/StateScoreVector.h"
#include <QuestZero/Common/RandomNumbers.h>

namespace Q0
{

/**
 * See http://en.wikipedia.org/wiki/Harmony_search
 * See https://sites.google.com/a/hydroteq.com/www/
 */
template<typename State, typename Score,
	typename ExitPolicy,
	typename InitializePolicy
>
struct HarmonySearch
: public ExitPolicy,
  public InitializePolicy
{
	typedef State state_t;
	typedef Score score_t;
	typedef Sample<state_t,score_t> sample_t;
	typedef StateScoreVector<state_t,score_t> sample_set_t;

	struct Parameters
	{
		Parameters()
		: hms(30),
		  hmcr(0.9),
		  par(0.3),
		  fw(0.01)
		{}

		unsigned int hms;
		double hmcr;
		double par;
		double fw;
	};

	Parameters parameters_;

	template<typename Space, typename Function, typename Compare, typename Visitor>
	sample_t Optimize(const Space& space, const Function& function, Compare cmp, Visitor vis) {
		// create harmony memory space and initialize with random samples
		sample_set_t hm;
		add_random_samples(hm, *this, space, parameters_.hms);
		// evaluate initial samples
		compute_likelihood(hm, function);
		// notify sample set observer
		vis.NotifySamples(hm);
		// run
		while(true) {
			// initialize a new state with random values
			state_t x = space.random();
			// merge with states from the harmony memory
			for(unsigned int i=0; i<space.dimension(); i++) { // FIXME dimension ???
				if(RandomNumbers::Uniform<double>() < parameters_.hmcr) {
					// pick random state from harmony memory
					unsigned int p = static_cast<unsigned int>(
							RandomNumbers::Uniform<double>() * static_cast<double>(parameters_.hms));
					if(p == parameters_.hms) p--;
					// use value from harmony memory
					space.component_copy(x, i, get_state(hm, p));
					// do more
					if(RandomNumbers::Uniform<double>() < parameters_.par) {
						// add a small randomness to x
						space.component_add_noise(x, i, parameters_.fw * RandomNumbers::Uniform<double>(-1,+1));
					}
				}
			}
			// evaluate the new particle
			score_t s = function(x);
			// if it is better than the worst in of the harmony memory -> replace
			auto worst_id = find_worst_by_score(hm, cmp);
			if(cmp(s, get_score(hm, worst_id))) {
				set_state(hm, worst_id, x);
				set_score(hm, worst_id, s);
			}
			// notify sample set observer
			vis.NotifySamples(hm);
			// find best particle
			auto best_id = find_best_by_score(hm, cmp);
			// check if it is good enough
			if(this->IsTargetReached(get_score(hm, best_id), cmp)) {
				// return best sample
				return get_sample(hm, best_id);
			}
		}
	}

};

}

#endif
