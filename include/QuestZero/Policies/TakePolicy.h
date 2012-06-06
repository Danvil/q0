/*
 * TakePolicy.h
 *
 *  Created on: Sep 10, 2010
 *      Author: david
 */

#ifndef QUESTZERO_TAKEPOLICY_H
#define QUESTZERO_TAKEPOLICY_H
//---------------------------------------------------------------------------
#include "QuestZero/Common/Sample.h"
#include "QuestZero/Common/SampleSet.h"
#include <functional>
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

namespace TakePolicy {

	//---------------------------------------------------------------------------

	/** Returns the state with best score from the set */
	template<typename State, typename Score>
	struct TakeBest
	{
		template<class Space, typename Compare>
		TSample<State,Score> take(const Space&, const TSampleSet<State,Score>& many, Compare cmp) {
			return get_sample(many, find_best_by_score(many, cmp));
		}

	protected:
		~TakeBest() {}
	};

	//---------------------------------------------------------------------------

	/** Returns a weighted mean of all sample states in the set weighted by sample score
	 * Weights are computed from scores using a functor.
	 * The default is to use the score as a weight.
	 * The sample set may contain different "groups" of samples. Building the
	 * mean between states of different groups may result in a state which is worse than
	 * both the original states!
	 */
	template<typename State, typename Score>
	struct TakeMean
	{
		typedef std::function<double(Score)> ScoreToWeightFunctor;

		TakeMean()
		: take_mean_max_count_(0),
		  ScoreToWeightFunctor([](Score s) { return s; })
		{}

		/** Sets the number of best samples to consider
		 * @param 0 means all samples. recommend is top 1-10%
		 */
		void SetTakeMeanMaxCount(unsigned int cnt) {
			take_mean_max_count_ = cnt;
		}

		template<class Space, class CMP, typename Compare>
		TSample<State,Score> take(const Space& space, const TSampleSet<State,Score>& many, Compare cmp) {
			size_t cnt = (take_mean_max_count_ == 0) ? num_samples(many) : std::min<size_t>(num_samples(many), take_mean_max_count_);
			TSampleSet<State,Score> best = pick_best(many, cnt, cmp);
			std::vector<Score> scores = get_score_list(best);
			std::vector<State> states_other = get_state_list(best);
			std::vector<State> states;
			std::vector<double> weight(scores.size());
			states.reserve(cnt);
			double weight_sum = 0;
			Score mixed_score = 0;
			for(size_t i=0; i<scores.size(); ++i) {
				states.push_back(states_other[i]); // convert to Space::State (the given state may by a derived from this)
				// FIXME why the second state list?
				Score s = scores[i];
				double w = fnc_score_to_weight_(s);
				weight[i] = w;
				weight_sum += w;
				mixed_score += w * s; // TODO is this legal?
			}
			double scl = 1.0 / weight_sum;
			mixed_score = scl * mixed_score; // TODO is this legal?
			for(size_t i=0; i<scores.size(); ++i) {
				weight[i] *= scl;
			}
			// weighted mean of samples
			// FIXME the score is of course not right! we should evaluate
			State mixed = space.weightedSum(weight, states);
			return TSample<State,Score>(mixed, mixed_score);
		}

	private:
		unsigned int take_mean_max_count_;

		ScoreToWeightFunctor fnc_score_to_weight_;

	protected:
		~TakeMean() {}
	};

	//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
