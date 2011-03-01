/*
 * TakePolicy.h
 *
 *  Created on: Sep 10, 2010
 *      Author: david
 */

#ifndef QUESTZERO_TAKEPOLICY_H
#define QUESTZERO_TAKEPOLICY_H
//---------------------------------------------------------------------------
#include "QuestZero/Common/ScoreComparer.h"
#include "QuestZero/Common/Sample.h"
#include "QuestZero/Common/SampleSet.h"
#include <Danvil/Tools/MoreMath.h>
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

namespace TakePolicy {

	//---------------------------------------------------------------------------

	/** Returns the state with best score from the set */
	template<typename State, typename Score>
	struct TakeBest
	{
		template<class Space, class CMP>
		const TSample<State,Score>& take(const Space&, const TSampleSet<State,Score>& many) {
			return many.template FindBestSample<CMP>();
		}

	protected:
		~TakeBest() {}
	};

	//---------------------------------------------------------------------------

	namespace Private {
		// this will yield a compiler error if this policy is used with something else than BetterMeansBigger
		template<class State, typename Score, class CMP> struct CmpChecker;
		template<class State, typename Score> struct CmpChecker<State, Score, BetterMeansBigger<Score> > {};
	}

	/** Returns a weighted mean of all sample states in the set weighted by sample score
	 * Warning: Works only with BetterMeansBigger SampleSets!
	 * Warning: The sample set may contain different "groups" of samples. Building the
	 * mean between states of different groups may result in a state which is worse than
	 * both the original states!
	 */
	template<typename State, typename Score>
	struct TakeMean
	{
		TakeMean() : take_mean_max_count_(0) {}

		/** Sets the number of best samples to consider
		 * @param 0 means all samples. recommend is top 1-10%
		 */
		void SetTakeMeanMaxCount(unsigned int cnt) {
			take_mean_max_count_ = cnt;
		}

		template<class Space, class CMP>
		TSample<State,Score> take(const Space& space, const TSampleSet<State,Score>& many) {
			Private::CmpChecker<State,Score, CMP>();
			size_t cnt = (take_mean_max_count_ == 0) ? many.count() : Danvil::MoreMath::Min<size_t>(many.count(), take_mean_max_count_);
			TSampleSet<State,Score> best = many.template FindBestSamplesAsSet<CMP>(cnt);
			std::vector<Score> scores = best.scores();
			std::vector<State> states_other = best.states();
			std::vector<typename Space::State> states;
			states.reserve(cnt);
			Score score_sum = 0;
			Score mixed_score = 0;
			for(size_t i=0; i<scores.size(); ++i) {
				states.push_back(states_other[i]); // convert to Space::State (the given state may by a derived from this)
				Score x = scores[i];
				score_sum += x;
				mixed_score += x * x; // wired ....
			}
			Score scl = 1.0 / score_sum;
			mixed_score *= scl; // same as with the states!
			for(size_t i=0; i<scores.size(); ++i) {
				scores[i] *= scl;
			}
			// weighted mean of samples
			// FIXME the score is of course not right! we should evaluate
			State mixed = space.weightedSum(scores, states);
			return TSample<State,Score>(mixed, mixed_score);
		}

	private:
		unsigned int take_mean_max_count_;

	protected:
		~TakeMean() {}
	};

	//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
