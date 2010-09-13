/*
 * ParticleFilter.h
 *
 *  Created on: Sep 12, 2010
 *      Author: david
 */

#ifndef PARTICLEFILTER_H_
#define PARTICLEFILTER_H_

#include <vector>

template<typename Time, typename State, class VarFunction>
struct PinnedFunction
{
	PinnedFunction(const VarFunction& f, const Time& t)
	: f_(f),
	  time_(t)
	{}

	void setTime(const Time& t) { time_ = t; }

	double operator()(const State& state) const {
		return f_(time_, state);
	}

	std::vector<double> operator()(const std::vector<State>& states) const {
		return f_(time_, states);
	}

private:
	VarFunction f_;
	Time time_;
};

#include "QuestZero/Common/SampleSet.h"
#include "QuestZero/Tracking/Solution.h"

template<typename Time, typename State, class StartingStates, class Take, class Tracer>
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
			// apply motion model which is simply white noise
			open_samples.addNoise(space, noise_);
			// evaluate samples
			open_samples.evaluateAll(pinned);
			// create new sample set using weighted random drawing
			try{
				open_samples = open_samples.drawByScore(particle_count_);
			} catch(typename SampleSet::CanNotNormalizeZeroListException& e) {
				// tracker lost the object
				return sol;
			}
			// tracing
			this->trace(open_samples);
			// save best sample
			sol.set(t, this->template take<Space, CMP>(space, open_samples));
		}
		return sol;
	}
};

#endif
