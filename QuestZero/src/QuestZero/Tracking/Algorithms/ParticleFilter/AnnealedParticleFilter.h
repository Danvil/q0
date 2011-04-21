/*
 * AnnealedParticleFilter.h
 *
 *  Created on: April 20, 2011
 *      Author: david
 */

#ifndef Q0_TRACKING_ALGORITHMS_ANNEALEDPARTICLEFILTER_H_
#define Q0_TRACKING_ALGORITHMS_ANNEALEDPARTICLEFILTER_H_
//---------------------------------------------------------------------------
#include "QuestZero/Spaces/Multiplier.h"
#include "QuestZero/Tracking/Functions.h"
#include "QuestZero/Common/ScoreComparer.h"
#include "QuestZero/Common/SampleSet.h"
#include "QuestZero/Tracking/Solution.h"
#include "QuestZero/Policies/TracePolicy.h"
#include "QuestZero/Optimization/Algorithms/Annealing.h"
#include "QuestZero/Tracking/MotionModel.h"
#include <vector>
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

struct AnnealedParticleFilterParameters
{
	AnnealedParticleFilterParameters() {
		particle_count_ = 1000;
		alpha_ = 0.5f;
		max_layers_ = 10;
	}

	unsigned int particle_count_;
	float alpha_;
	unsigned int max_layers_;

};

/** Annealed particle filter
 * The maximal number of function evaluations per time step is: particle count * maximal layer count
 */
template<typename Time, typename State, typename Score, class StartingStates, class Take, class NotifySamples, class NotifySolution>
struct AnnealedParticleFilter
: public StartingStates,
  public Take,
  public NotifySamples,
  public NotifySolution
{
	typedef TSolution<Time, State, Score> Solution;
	typedef TSampleSet<State, Score> SampleSet;

	AnnealedParticleFilterParameters params_;

	template<class Space, class VarFunction, class MotionModel>
	Solution Track(const Solution& solution, const Space& space, const VarFunction& function, MotionModel motion) {
		typedef BetterMeansBigger<Score> CMP;
		Solution sol = solution;
		// pin down varying function
		PinnedFunction<Time, State, Score, VarFunction> pinned(function);
		// initialize sample set with random samples
		// Remark: We use one sample and the same sample set for all individual optimizations
		//         This way the optimizations are not completely independent, but as the noise
		//         at the beginning should be low it should not matter.
		//         Perhaps it is even better ...
		SampleSet open_samples;
		// tracking loop
		typename Solution::It tt_start = sol.GetFirstUnknown();
		for(typename Solution::It tt=tt_start; tt!=sol.GetEnd(); ++tt) {
			// set pin down time
			pinned.setTime(tt->time);
			// evaluate the initially picked samples
			if(tt == tt_start) {
				// generate initial sample set
				open_samples = this->pickMany(space, params_.particle_count_);
			}
			else {
				// apply motion model to states
				motion.SetNoiseAmount(1);
				open_samples.TransformStates(motion);
			}
			// use annealing to refine the sample set
			ParticleAnnealing<State, Score, TracePolicy::Samples::ForwardToObject<State,Score,NotifySamples> > pa;
			pa.SetNotifySamplesObject(this);
			pa.settings_.layers_ = params_.max_layers_;
			pa.settings_.particle_count_ = params_.particle_count_;
			pa.settings_.alpha_ = params_.alpha_;
			pa.OptimizeInplace(open_samples, space, pinned, motion);
			// save best sample
			sol.Set(tt->index, this->template take<Space, CMP>(space, open_samples));
			// tracing
			this->NotifySolution(sol);
		}
		return sol;
	}

	template<class Space, class VarFunction>
	Solution TrackWhiteNoise(const Solution& sol, const Space& space, const VarFunction& function, const std::vector<double>& noise) {
		MotionModels::SpaceRandomMotionModel<Space> motion(space, noise);
		return Track(sol, space, function, motion);
	}

};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
