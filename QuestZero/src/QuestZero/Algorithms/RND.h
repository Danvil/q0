/*
 * RND.h
 *
 *  Created on: Sep 4, 2010
 *      Author: david
 */

#ifndef RND_H_
#define RND_H_

#include "../StartingValues.h"
#include "../SampleSet.h"
#include "../IAlgorithm.h"
#include <Danvil/Tools/Small.h>
#include <string>

/// <summary>
/// A optimization algorithm which tries to find the minimum of a function with stupid randomness
/// </summary>
template<typename Problem>
class RND
: public IMinimizationAlgorithm<Problem>
{
public:
	typedef typename Problem::State State;
	typedef TSample<State> Sample;
	typedef TSampleSet<State> SampleSet;
	typedef typename Problem::StateOperator Op;
	typedef typename Problem::Domain Domain;
	typedef typename Problem::Function Function;
	typedef typename Problem::Tracer Tracer;

public:
	const std::string& name() const { return "RND"; }

	RND() {
		particleCount = 100;
		maxIterations = 10;
	}

	virtual ~RND() {}

	unsigned int particleCount;
	unsigned int maxIterations;

	SampleSet Optimize(PTR(Domain) dom, PTR(Function) f, const std::vector<State>& given_initial_states, PTR(Tracer) tracer) {
		std::vector<State> complete_initial_states = StartingValues::Repeat(given_initial_states, particleCount);
		SampleSet open(complete_initial_states);
		// in every iteration add new particles and delete the worst particles
		for(unsigned int k = 1; k < maxIterations; k++) {
			// add new samples by randomly selecting points
			int target_add = Danvil::max((size_t)0, 2 * particleCount - open.count());
			open.add(dom->random(target_add));
			// evaluate the chunk
			open.evaluateUnknown(f);
			// pick the best
			// check if the best in this chunk is better than the best so far
			open = open.best(particleCount);
			// update progress bar
			tracer->trace(k + 1, maxIterations, open);
			// check if best satisfy break condition
//			if(Settings.Finished(k + 1, open.Best().Score))
//				break;
		}
		// return last best samples
		return open;
	}
};

#endif
