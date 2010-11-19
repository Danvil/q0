/*
 * ParticleFilter.h
 *
 *  Created on: Sep 12, 2010
 *      Author: david
 */

#ifndef PARTICLEFILTER_H_
#define PARTICLEFILTER_H_

#include "QuestZero/Tracking/PinnedFunction.h"
#include "QuestZero/Common/ScoreComparer.h"
#include "QuestZero/Common/SampleSet.h"
#include "QuestZero/Tracking/Solution.h"
#include "QuestZero/Optimization/Algorithms/Annealing.h"

template<typename Time, typename State, class StartingStates, class Take, class Tracer, bool UseAnnealing=false>
struct ParticleFilter
: public StartingStates,
  public Take,
  public Tracer
{
	typedef TSolution<Time, State> Solution;
	typedef TTimeRange<Time> TimeRange;
	typedef TSampleSet<State> SampleSet;

	unsigned long particle_count_;

	std::vector<double> noise_;

	template<class Space, class VarFunction>
	Solution track(const TimeRange& range, const Space& space, const VarFunction& function) {
		typedef BetterMeansBigger<State> CMP;
		// prepare the solution
		Solution sol(range);
		// pin down varying function
		PinnedFunction<Time,State,VarFunction> pinned(function, range.begin());
		// initialize sample set with random samples
		SampleSet open_samples(this->pickMany(space, particle_count_));
		// tracking loop
		for(Time t = range.begin(); t != range.end(); ++t) {
			// set pin down time
			pinned.setTime(t);
			if(UseAnnealing) {
				// use annealing to refine the sample set
				ParticleAnnealing<State, Tracer> pa;
				pa.settings_.noise_ = noise_;
				open_samples = pa.optimize(open_samples, space, pinned);
			} else {
				// apply motion model which is simply white noise
				open_samples.addNoise(space, noise_);
				// evaluate samples
				open_samples.evaluateAll(pinned);
				// create new sample set using weighted random drawing
				try{
					open_samples = open_samples.drawByScore(particle_count_);
				} catch(typename SampleSet::CanNotNormalizeZeroListException&) {
					// tracker lost the object
					return sol;
				}
			}
			// tracing
			this->trace(open_samples);
			// save best sample
			sol.set(t, this->template take<Space, CMP>(space, open_samples));
		}
		return sol;
	}
};

template<typename Time, typename State, class StartingStates, class Take, class Tracer>
struct ParticleFilterWithAnnealing
: public ParticleFilter<Time, State, StartingStates, Take, Tracer>
{};

#endif
