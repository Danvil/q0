/*
 * ParticleFilter.h
 *
 *  Created on: Sep 12, 2010
 *      Author: david
 */

#ifndef PARTICLEFILTER_H_
#define PARTICLEFILTER_H_
//---------------------------------------------------------------------------
#include "QuestZero/Tracking/Functions.h"
#include "QuestZero/Common/ScoreComparer.h"
#include "QuestZero/Common/SampleSet.h"
#include "QuestZero/Tracking/Solution.h"
#include "QuestZero/Optimization/Algorithms/Annealing.h"
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

template<typename Time, typename State, typename Score, class StartingStates, class Take, class NotifySamples, class NotifySolution, bool UseAnnealing>
struct ParticleFilter
: public StartingStates,
  public Take,
  public NotifySamples,
  public NotifySolution
{
	typedef TSolution<Time, State, Score> Solution;
	typedef TTimeRange<Time> TimeRange;
	typedef TSampleSet<State, Score> SampleSet;

	unsigned long particle_count_;

	std::vector<double> noise_;

	template<class Space, class VarFunction>
	Solution Track(const TimeRange& range, const Space& space, const VarFunction& function) {
		typedef BetterMeansBigger<Score> CMP;
		// prepare the solution
		Solution sol(range);
		// pin down varying function
		PinnedFunction<Time, State, Score, VarFunction> pinned(function, range.begin());
		// initialize sample set with random samples
		SampleSet open_samples(this->pickMany(space, particle_count_));
		// tracking loop
		for(Time t = range.begin(); t != range.end(); ++t) {
			// set pin down time
			pinned.setTime(t);
			if(UseAnnealing) {
				// use annealing to refine the sample set
				ParticleAnnealing<State, Score, NotifySamples> pa;
				pa.settings_.noise_ = noise_;
				open_samples = pa.optimize(open_samples, space, pinned);
			} else {
				// apply motion model which is simply white noise
				open_samples.addNoise(space, noise_);
				// evaluate samples
				open_samples.evaluateAll(pinned);
				// create new sample set using weighted random drawing
				try{
					open_samples = open_samples.DrawByScore(particle_count_);
				} catch(typename SampleSet::CanNotNormalizeZeroListException&) {
					LOG_WARNING << "All samples have a score of zero. This means the tracker lost the object!";
					// tracker lost the object
					return sol;
				}
			}
			// save best sample
			sol.set(t, this->template take<Space, CMP>(space, open_samples));
			// tracing
			this->NotifySamples(open_samples);
			this->NotifySolution(sol);
		}
		return sol;
	}
};

template<typename Time, typename State, typename Score, class StartingStates, class Take, class NotifySamples, class NotifySolution>
struct ParticleFilterWithAnnealing
: public ParticleFilter<Time, State, Score, StartingStates, Take, NotifySamples, NotifySolution, true>
{};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
