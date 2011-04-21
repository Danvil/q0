/*
 * condensation.h
 *
 *  Created on: Sep 12, 2010
 *  Changed on: April 20, 2011
 *      Author: david
 */

#ifndef Q0_TRACKING_ALGORITHMS_CONDENSATION_H_
#define Q0_TRACKING_ALGORITHMS_CONDENSATION_H_
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

/** Condensation algorithm after Isard and Black
 * This algorithms tries to maximize the function values.
 * The number of function evaluations per time step is exactly equal to the number of particles.
 */
template<typename Time, typename State, typename Score, class StartingStates, class Take, class NotifySamples, class NotifySolution>
struct Condensation
: public StartingStates,
  public Take,
  public NotifySamples,
  public NotifySolution
{
	typedef TSolution<Time, State, Score> Solution;
	typedef TSampleSet<State, Score> SampleSet;

	unsigned long particle_count_;

	template<class Space, class VarFunction, class MotionModel>
	Solution Track(const Solution& solution, const Space& space, const VarFunction& function, MotionModel motion) {
		typedef BetterMeansBigger<Score> CMP;
		// prepare the solution
		Solution sol = solution;
		// pin down varying function
		PinnedFunction<Time, State, Score, VarFunction> pinned(function);
		// initialize sample set with random samples
		SampleSet open_samples(this->pickMany(space, particle_count_));
		// tracking loop
		typename Solution::ConstIt tt_start = sol.GetFirstUnknown();
		for(typename Solution::ConstIt tt=tt_start; tt!=sol.GetEnd(); ++tt) {
			// set pin down time
			pinned.setTime(tt->time);
			// apply motion model which is simply white noise
			open_samples.TransformStates(motion);
			// evaluate samples
			open_samples.EvaluateAll(pinned);
			// resample using weighted random drawing
			try{
				open_samples.Resample(particle_count_);
			} catch(typename SampleSet::CanNotNormalizeZeroListException&) {
				LOG_WARNING << "All samples have a score of zero. This means the tracker lost the object!";
				// tracker lost the object
				return sol;
			}
			// save best sample
			sol.Set(tt->index, this->template take<Space, CMP>(space, open_samples));
			// tracing
			this->NotifySamples(open_samples);
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
