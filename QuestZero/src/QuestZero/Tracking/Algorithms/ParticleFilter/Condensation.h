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
#include <Danvil/Tools/Benchmark.h>
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
	typedef TRange<Time, State, Score> Range;
	typedef TSampleSet<State, Score> SampleSet;

	unsigned long particle_count_;

	template<class Space, class VarFunction, class MotionModel>
	Solution Track(const Range& range, const Space& space, const VarFunction& function, MotionModel motion) {
		typedef BetterMeansBigger<Score> CMP;
		// prepare the solution
		Solution sol = range.solution_;
		// pin down varying function
		PinnedFunction<Time, State, Score, VarFunction> pinned(function);
		// initialize sample set with random samples
		SampleSet open_samples(this->pickMany(space, particle_count_));
		// tracking loop (evaluate from first unknown til next known)
		for(size_t tt=range.begin_; tt<range.end_; tt+=range.strive_) {
			DANVIL_BENCHMARK_START(Q0_Condensation)
			// set pin down time
//			DANVIL_BENCHMARK_START(Q0_Condensation_SetTime)
			pinned.setTime(sol.GetTime(tt));
//			DANVIL_BENCHMARK_STOP(Q0_Condensation_SetTime)
			// apply motion model which is simply white noise
//			DANVIL_BENCHMARK_START(Q0_Condensation_Transform)
			open_samples.TransformStates(motion);
//			DANVIL_BENCHMARK_STOP(Q0_Condensation_Transform)
			// evaluate samples
			DANVIL_BENCHMARK_START(Q0_Condensation_Likelihood)
			open_samples.ComputeLikelihood(pinned);
			DANVIL_BENCHMARK_STOP(Q0_Condensation_Likelihood)
			// notify samples
			DANVIL_BENCHMARK_START(Q0_Condensation_NotifySamples)
			this->NotifySamples(open_samples);
			DANVIL_BENCHMARK_STOP(Q0_Condensation_NotifySamples)
			// resample using weighted random drawing
//			DANVIL_BENCHMARK_START(Q0_Condensation_Resample)
			try{
				open_samples.Resample(particle_count_);
			} catch(CanNotNormalizeZeroListException&) {
				LOG_WARNING << "All samples have a score of zero. This means the tracker lost the object!";
				// tracker lost the object
				return sol;
			}
//			DANVIL_BENCHMARK_STOP(Q0_Condensation_Resample)
			// save best sample
			sol.Set(tt, this->template take<Space, CMP>(space, open_samples));
			// tracing
			DANVIL_BENCHMARK_START(Q0_Condensation_NotifySolution)
			this->NotifySolution(sol);
			DANVIL_BENCHMARK_STOP(Q0_Condensation_NotifySolution)
			DANVIL_BENCHMARK_STOP(Q0_Condensation)
		}
		return sol;
	}

	template<class Space, class VarFunction>
	Solution TrackWhiteNoise(const Range& range, const Space& space, const VarFunction& function, const std::vector<double>& noise) {
		MotionModels::SpaceRandomMotionModel<Space> motion(space, noise);
		return Track(range, space, function, motion);
	}

};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
