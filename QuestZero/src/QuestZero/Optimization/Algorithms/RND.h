/*
 * RND.h
 *
 *  Created on: Sep 4, 2010
 *      Author: david
 */

#ifndef ALGORITHMS_RND_H
#define ALGORITHMS_RND_H

#include "QuestZero/Common/Sample.h"
#include "QuestZero/Common/SampleSet.h"
#include <Danvil/Tools/Small.h>
#include <string>

/// <summary>
/// A optimization algorithm which tries to find the minimum of a function with stupid randomness
/// </summary>
template<
	typename State,
	class Target,
	class StartingStates,
	class Take,
	class Tracer
>
struct RND
: public Target,
  public StartingStates,
  public Take,
  public Tracer
{
	RND() {
		particleCount = 100;
	}

	virtual ~RND() {}

	std::string name() const { return "RND"; }

	unsigned int particleCount;

	template<class Space, class Function>
	TSample<State> optimize(const Space& space, const Function& function) {
		typedef BetterMeansSmaller<State> CMP;
		TSampleSet<State> open(this->pickMany(space, particleCount));
		// in every iteration add new particles and delete the worst particles
		while(true) {
			// add new samples by randomly selecting points
			int target_add = Danvil::max((size_t)0, 2 * particleCount - open.count());
			open.add(space.randomMany(target_add));
			// evaluate the chunk
			open.evaluateUnknown(function);
			// pick the best
			// check if the best in this chunk is better than the best so far
			open = open.template best<CMP>(particleCount);
			// update progress bar
			this->trace(open);
			// check if break condition is satisfied
			if(this->reached(open.template best<CMP>().score())) {
				break;
			}
		}
		// return last best samples
		return this->template take<Space, CMP>(space, open);
	}
};

#endif
