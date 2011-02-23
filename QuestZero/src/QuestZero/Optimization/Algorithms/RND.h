/*
 * RND.h
 *
 *  Created on: Sep 4, 2010
 *      Author: david
 */

#ifndef ALGORITHMS_RND_H
#define ALGORITHMS_RND_H
//---------------------------------------------------------------------------
#include "QuestZero/Common/ScoreComparer.h"
#include "QuestZero/Common/Sample.h"
#include "QuestZero/Common/SampleSet.h"
#include <Danvil/Tools/MoreMath.h>
#include <string>
#include <vector>
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

/// <summary>
/// A optimization algorithm which tries to find the minimum of a function with stupid randomness
/// </summary>
template<
	typename State,
	class Score,
	class Target,
	class StartingStates,
	class Take,
	class NotifySamples
>
struct RND
: public Target,
  public StartingStates,
  public Take,
  public NotifySamples
{
	typedef TSample<State,Score> Sample;
	typedef TSampleSet<State,Score> SampleSet;

	RND() {
		particleCount = 100;
	}

	virtual ~RND() {}

	std::string name() const { return "RND"; }

	unsigned int particleCount;

	template<class Space, class Function>
	Sample Optimize(const Space& space, const Function& function) {
		typedef BetterMeansSmaller<Score> CMP;
		SampleSet open(this->template pickMany(space, particleCount));
		// in every iteration add new particles and delete the worst particles
		while(true) {
			// add new samples by randomly selecting points
			int target_add = Danvil::MoreMath::Max((size_t)0, 2 * particleCount - open.count());
			open.AddManyStates(space.template randomMany(target_add));
			// evaluate the chunk
			open.evaluateUnknown(function);
			// pick the best
			// check if the best in this chunk is better than the best so far
			open = open.template FindBestSamples<CMP>(particleCount);
			// update progress bar
			this->NotifySamples(open);
			// check if break condition is satisfied
			if(this->IsTargetReached(open.template FindBestScore<CMP>())) {
				break;
			}
		}
		// return last best samples
		return this->template take<Space, CMP>(space, open);
	}
};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
