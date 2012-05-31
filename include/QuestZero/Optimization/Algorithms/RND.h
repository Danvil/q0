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
	class NotifySamples,
	bool Minimize
>
struct RND
: public Target,
  public StartingStates,
  public Take,
  public NotifySamples
{
	typedef TSample<State,Score> Sample;
	typedef TSampleSet<State,Score> SampleSet;
	typedef typename ComparerSelector<Score,Minimize>::Result CMP;

	RND() {
		particleCount = 100;
	}

	virtual ~RND() {}

	std::string name() const { return "RND"; }

	unsigned int particleCount;

	template<class Space, class Function>
	Sample Optimize(const Space& space, const Function& function) {
		SampleSet open(this->template pickMany(space, particleCount));
		// evaluate initial samples
		compute_likelihood(open, function);
		// update progress bar
		this->NotifySamples(open);
		// in every iteration add new particles, delete the worst particles
		// iterate until a given condition is fulfilled
		auto best_id = find_best_by_score(open, CMP());
		while(!this->IsTargetReached(get_score(open, best_id))) {
			// add new samples by randomly selecting points
			assert(num_samples(open) == particleCount);
			// generate new chunk of states
			SampleSet new_samples(space.template randomMany(particleCount));
//			SampleSet new_samples(this->template pickMany(space, particleCount));
			// evaluate the chunk
			compute_likelihood(new_samples, function);
			// add to open samples
			add_samples(open, new_samples);
			// pick the best
			open = pick_best(open, particleCount, CMP());
			// update progress bar
			this->NotifySamples(open);
		}
		// return last best samples
		return this->template take<Space, CMP>(space, open);
	}
};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
