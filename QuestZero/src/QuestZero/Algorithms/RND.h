/*
 * RND.h
 *
 *  Created on: Sep 4, 2010
 *      Author: david
 */

#ifndef ALGORITHMS_RND_H
#define ALGORITHMS_RND_H

#include "../Sample.h"
#include "../SampleSet.h"
#include <Danvil/Tools/Small.h>
#include <string>

/// <summary>
/// A optimization algorithm which tries to find the minimum of a function with stupid randomness
/// </summary>
template<
	typename State,
	class StartingStates,
	class Take,
	class Tracer
>
struct RND
: public StartingStates,
  public Take,
  public Tracer
{
	RND() {
		particleCount = 100;
		maxIterations = 10;
	}

	virtual ~RND() {}

	std::string name() const { return "RND"; }

	unsigned int particleCount;
	unsigned int maxIterations;

	template<class Space, class Function>
	TSample<State> optimize(const Space& space, PTR(Function) function) {
		TSampleSet<State> open(this->pickMany(space, particleCount));
		// in every iteration add new particles and delete the worst particles
		for(unsigned int k = 1; k < maxIterations; k++) {
			// add new samples by randomly selecting points
			int target_add = Danvil::max((size_t)0, 2 * particleCount - open.count());
			open.add(space.randomMany(target_add));
			// evaluate the chunk
			open.evaluateUnknown(function);
			// pick the best
			// check if the best in this chunk is better than the best so far
			open = open.best(particleCount);
			// update progress bar
			this->trace(k + 1, maxIterations, open);
			// check if best satisfy break condition
//			if(Settings.Finished(k + 1, open.Best().Score))
//				break;
		}
		// return last best samples
		return this->take(space, open);
	}
};

#endif
