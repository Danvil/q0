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
			return many.template best<CMP>();
		}

	protected:
		~TakeBest() {}
	};

	//---------------------------------------------------------------------------

	// this will yield a compiler error if this policy is used with something else than BetterMeansBigger
	template<class State, typename Score, class CMP> struct CmpChecker;
	template<class State, typename Score> struct CmpChecker<State, Score, BetterMeansBigger<State,Score> > {};

	/** Returns a weighted mean of all sample states in the set weighted by sample score
	 * Works only with BetterMeansBigger SampleSets!
	 */
	template<typename State, typename Score>
	struct TakeMean
	{
		template<class Space, class CMP>
		TSample<State,Score> take(const Space& space, const TSampleSet<State,Score>& many) {
			CmpChecker<State,Score, CMP>(); // test
			// FIXME this weighting assumes, that greater scores are better ... but what else do we have ?
			std::vector<double> scores_normalized = many.scores();
			double score_sum = 0;
			double mixed_score = 0;
			for(size_t i=0; i<scores_normalized.size(); ++i) {
				double x = scores_normalized[i];
				score_sum += x;
				mixed_score += x * x; // wired ....
			}
			double scl = 1.0 / score_sum;
			mixed_score *= scl; // same as with the states!
			for(size_t i=0; i<scores_normalized.size(); ++i) {
				scores_normalized[i] *= scl;
			}
			return TSample<State,Score>(space.weightedSum(scores_normalized, many.states()), mixed_score);
		}

	protected:
		~TakeMean() {}
	};

	//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
