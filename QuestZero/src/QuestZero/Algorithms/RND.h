/*
 * RND.h
 *
 *  Created on: Sep 4, 2010
 *      Author: david
 */

#ifndef RND_H_
#define RND_H_

#include "../IAlgorithm.h"
#include <string>

/// <summary>
/// A optimization algorithm which tries to find the minimum of a function with stupid randomness
/// </summary>
template<typename Traits>
class RND
	: IMinimizationAlgorithm<Traits>
{
public:
	typedef typename Traits::State State;
	typedef typename Traits::SampleSet SampleSet;
	typedef typename Traits::Function Function;

public:
	const std::string& name() const { return "RND"; }

	RND() {
		particleCount = 100;
		maxIterations = 10;
	}

	virtual ~RND() {}

	unsigned int particleCount;
	unsigned int maxIterations;

	SampleSet Optimize(PTR(Function) f) {
		SampleSet open;
		// in every iteration add new particles and delete the worst particles
		for(int k = 1; k < maxIterations; k++) {
			// add new samples by randomly selecting points
			int target_add = max(0, 2 * particleCount - open.size());
			open.add(State::Random(target_add));
			// evaluate the chunk
			open.evaluateUnkown(f);
			// pick the best
			// check if the best in this chunk is better than the best so far
			open = open.best(particleCount);
			// update progress bar
//			Trace(k + 1, Settings.MaxIterations, open);
			// check if best satisfy break condition
//			if(Settings.Finished(k + 1, open.Best().Score))
//				break;
		}
		// return last best samples
		return open;
	}
};

#endif
