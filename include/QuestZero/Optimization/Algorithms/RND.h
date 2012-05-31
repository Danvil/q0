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
		open.ComputeLikelihood(function);
		// update progress bar
		this->NotifySamples(open);
		// in every iteration add new particles, delete the worst particles
		// iterate until a given condition is fulfilled
		while(!this->IsTargetReached(open.template FindBestScore<CMP>())) {
			// add new samples by randomly selecting points
			assert(open.Size() == particleCount);
			// generate new chunk of states
			SampleSet new_samples(space.template randomMany(particleCount));
//			SampleSet new_samples(this->template pickMany(space, particleCount));
			// evaluate the chunk
			new_samples.ComputeLikelihood(function);
			// add to open samples
			open.Add(new_samples);
			// pick the best
			open = open.template FindBestSamples<CMP>(particleCount);
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
