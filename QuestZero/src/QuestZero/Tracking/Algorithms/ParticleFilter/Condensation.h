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
template<typename State, typename Score, class TimeControl, class StartingStates, class Take, class NotifySamples, class NotifySolution>
struct Condensation
: public TimeControl,
  public StartingStates,
  public Take,
  public NotifySamples,
  public NotifySolution
{
	typedef TSampleSet<State, Score> SampleSet;

	unsigned long particle_count_;

	template<class Space, class VarFunction, class MotionModel>
	void Track(const Space& space, const VarFunction& function, MotionModel motion) {
		typedef BetterMeansBigger<Score> CMP;
		// pin down varying function
		PinnedFunction<unsigned int, State, Score, VarFunction> pinned(function);
		// initialize sample set with random samples
		SampleSet open_samples(this->pickMany(space, particle_count_));
		// tracking loop (evaluate from first unknown til next known)
		for(unsigned int i=0; this->isRunning(i); i++) {
			DANVIL_BENCHMARK_START(Q0_Condensation)
			// set pin down time
//			DANVIL_BENCHMARK_START(Q0_Condensation_SetTime)
			pinned.setTime(i);
//			DANVIL_BENCHMARK_STOP(Q0_Condensation_SetTime)
			// apply motion model which is simply white noise
//			DANVIL_BENCHMARK_START(Q0_Condensation_Transform)
			open_samples.TransformStates(motion);
//			DANVIL_BENCHMARK_STOP(Q0_Condensation_Transform)
			// evaluate samples
			DANVIL_BENCHMARK_START(Q0_Condensation_Likelihood)
			open_samples.ComputeLikelihood(pinned);
			DANVIL_BENCHMARK_STOP(Q0_Condensation_Likelihood)
			// resample using weighted random drawing
//			DANVIL_BENCHMARK_START(Q0_Condensation_Resample)
			try{
				open_samples.Resample(particle_count_);
			} catch(CanNotNormalizeZeroListException&) {
				LOG_WARNING << "All samples have a score of zero. This means the tracker lost the object!";
				// tracker lost the object
				return;
			}
//			DANVIL_BENCHMARK_STOP(Q0_Condensation_Resample)
			// notify all samples
			DANVIL_BENCHMARK_START(Q0_Condensation_NotifySamples)
			this->NotifySamples(i, open_samples);
			DANVIL_BENCHMARK_STOP(Q0_Condensation_NotifySamples)
			// notify picked best sample
			DANVIL_BENCHMARK_START(Q0_Condensation_NotifySolution)
			this->NotifySolution(i, this->template take<Space, CMP>(space, open_samples));
			DANVIL_BENCHMARK_STOP(Q0_Condensation_NotifySolution)
			DANVIL_BENCHMARK_STOP(Q0_Condensation)
		}
	}

	template<class Space, class VarFunction>
	void TrackWhiteNoise(const Space& space, const VarFunction& function, const std::vector<double>& noise) {
		MotionModels::SpaceRandomMotionModel<Space> motion(space, noise);
		Track(space, function, motion);
	}

};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
