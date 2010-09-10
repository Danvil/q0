/*
 * TakePolicy.h
 *
 *  Created on: Sep 10, 2010
 *      Author: david
 */

#ifndef QUESTZERO_TAKEPOLICY_H
#define QUESTZERO_TAKEPOLICY_H
//---------------------------------------------------------------------------
#include "Sample.h"
#include "SampleSet.h"
//---------------------------------------------------------------------------

namespace TakePolicy {

	//---------------------------------------------------------------------------

	/** Returns the state with best score from the set */
	template<typename State>
	struct TakeBest
	{
		template<class Space>
		const TSample<State>& take(const Space&, const TSampleSet<State>& many) {
			return many.best();
		}
	};

	//---------------------------------------------------------------------------

	/** Returns a weighted mean of all sample states in the set weighted by sample score */
	template<typename State>
	struct TakeMean
	{
		template<class Space>
		TSample<State> take(const Space& space, const TSampleSet<State>& many) {
			return space.weightedSum(many.scores(), many.states());
		}
	};

	//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------
#endif
