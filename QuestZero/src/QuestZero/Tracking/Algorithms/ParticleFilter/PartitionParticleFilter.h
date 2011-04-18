/*
 * ParticleFilter.h
 *
 *  Created on: Sep 12, 2010
 *      Author: david
 */

#ifndef Q0_SOLVER_TRACKING_ALTERNATINGPARTICLEFILTER_H_
#define Q0_SOLVER_TRACKING_ALTERNATINGPARTICLEFILTER_H_
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

struct Partition
{
	std::vector<size_t> state_ids_;
	unsigned long particle_count_;
	unsigned long annealing_layers_;
	std::string name_;
};

struct PartitionParticleFilterParameters
{
	PartitionParticleFilterParameters() {
		repetition_count_ = 1;
		noise_suppression_factor_ = 0.05;
	}

	unsigned int repetition_count_;

	double noise_suppression_factor_;

	std::vector<Partition> partitions_;

};

/** A hierarchical particle filter uses partitions to subdivide the state vector
 *
 * Total number of evaluations per time step:
 * 	# = RepetitionCount * sum_{i=1}^{PartitionCount}{ParticelCount_i * AnnealingLayerCount_i}
 */
template<typename Time, typename State, typename Score, class StartingStates, class Take, class NotifySamples, class NotifySolution, bool UseAnnealing>
struct PartitionParticleFilter
: public StartingStates,
  public Take,
  public NotifySamples,
  public NotifySolution
{
	typedef TSolution<Time, State, Score> Solution;
	typedef TSampleSet<State, Score> SampleSet;

	PartitionParticleFilterParameters params_;

	static const unsigned int cInitialCount_ = 1000;

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
		SampleSet open_samples(this->pickMany(space, cInitialCount_));
		// tracking loop
		typename Solution::It tt_start = sol.GetFirstUnknown();
		for(typename Solution::It tt=tt_start; tt!=sol.GetEnd(); ++tt) {
			// set pin down time
			pinned.setTime(tt->time);
			// evaluate the initially picked samples
			if(tt == tt_start) {
				open_samples.EvaluateAll(pinned);
			}
			// several iterations
			for(unsigned int repetition=0; repetition<params_.repetition_count_; repetition++) {
				// one part after the other
				for(std::vector<Partition>::const_iterator it=params_.partitions_.begin(); it!=params_.partitions_.end(); ++it) {
					std::cout << "Partition " << (it - params_.partitions_.begin()) << " '" << it->name_ << "'" << std::endl;
					// restrict noise to partition
					motion.SetPartition(it->state_ids_, params_.noise_suppression_factor_);
					// run optimization
					if(UseAnnealing) {
						// use annealing to refine the sample set
						ParticleAnnealing<State, Score, TracePolicy::Samples::ForwardToObject<State,Score,NotifySamples> > pa;
						pa.SetNotifySamplesObject(this);
						pa.settings_.layers_ = it->annealing_layers_;
						pa.settings_.particle_count_ = it->particle_count_;
						pa.OptimizeInplace(open_samples, space, pinned, motion);
					} else {
						// apply motion model which is simply white noise
						open_samples.TransformStates(motion);
						// evaluate samples
						open_samples.EvaluateAll(pinned);
						// create new sample set using weighted random drawing
						try{
							open_samples = open_samples.DrawByScore(it->particle_count_);
						} catch(typename SampleSet::CanNotNormalizeZeroListException&) {
							LOG_WARNING << "All samples have a score of zero. This means the tracker lost the object!";
							// tracker lost the object
							return sol;
						}
					}
					// notify samples
					this->NotifySamples(open_samples);
				}
			}
			// save best sample
			sol.Set(tt->index, this->template take<Space, CMP>(space, open_samples));
			// tracing
			this->NotifySamples(open_samples);
			this->NotifySolution(sol);
		}
		return sol;
	}

//	template<class Space, class VarFunction, class MotionModel>
//	Solution Track(const TimeRange& range, const Space& space, const VarFunction& function, MotionModel motion) {
//		return Track(Solution(range), space, function, motion);
//	}

	template<class Space, class VarFunction>
	Solution TrackWhiteNoise(const Solution& sol, const Space& space, const VarFunction& function, const std::vector<double>& noise) {
		MotionModels::SpaceRandomMotionModel<Space> motion(space, noise);
		return Track(sol, space, function, motion);
	}

//	template<class Space, class VarFunction>
//	Solution TrackWhiteNoise(const TimeRange& range, const Space& space, const VarFunction& function, const std::vector<double>& noise) {
//		MotionModels::SpaceRandomMotionModel<Space> motion(space, noise);
//		return Track(range, space, function, motion);
//	}

};

template<typename Time, typename State, typename Score, class StartingStates, class Take, class NotifySamples, class NotifySolution>
struct PartitionParticleFilterWithAnnealing
: public PartitionParticleFilter<Time, State, Score, StartingStates, Take, NotifySamples, NotifySolution, true>
{};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
