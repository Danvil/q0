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
#include "QuestZero/Policies/TracePolicy.h"
#include "QuestZero/Optimization/Algorithms/Annealing.h"
#include "QuestZero/Tracking/MotionModel.h"
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

	template<class Space, class VarFunction, class MotionModel>
	Solution Track(const TimeRange& range, const Space& space, const VarFunction& function, MotionModel motion) {
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
				ParticleAnnealing<State, Score, TracePolicy::Samples::ForwardToObject<State,Score,NotifySamples> > pa;
				pa.SetNotifySamplesObject(this);
				pa.OptimizeInplace(open_samples, space, pinned, motion);
			} else {
				// apply motion model which is simply white noise
//				open_samples.RandomizeStates(space, noise_);
				open_samples.TransformStates(motion);
				// evaluate samples
				open_samples.EvaluateAll(pinned);
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

	template<class Space, class VarFunction>
	Solution TrackWhiteNoise(const TimeRange& range, const Space& space, const VarFunction& function, const std::vector<double>& noise) {
		MotionModels::SpaceRandomMotionModel<Space> motion(space, noise);
		return Track(range, space, function, motion);
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
