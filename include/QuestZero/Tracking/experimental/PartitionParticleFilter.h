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
	std::vector<size_t> noise_indices_;
	unsigned long particle_count_;
	unsigned long annealing_layers_;
	float alpha_;
	std::string name_;
};

struct PartitionParticleFilterParameters
{
	PartitionParticleFilterParameters() {
		repetition_count_ = 1;
		particle_count_ = 1024;
		noise_suppression_factor_ = 0.05;
	}

	unsigned int repetition_count_;

	unsigned int particle_count_;

	double noise_suppression_factor_;

	std::vector<Partition> partitions_;

};

/** A hierarchical particle filter uses partitions to subdivide the state vector
 *
 * Total number of evaluations per time step:
 * 	# = RepetitionCount * sum_{i=1}^{PartitionCount}{ParticelCount_i * AnnealingLayerCount_i}
 */
template<typename State, typename Score, class TimeControl, class StartingStates, class Take, class NotifySamples, class TimestepResult, bool UseAnnealing>
struct PartitionParticleFilter
: public TimeControl,
  public StartingStates,
  public Take,
  public NotifySamples,
  public TimestepResult
{
	typedef TSampleSet<State, Score> SampleSet;

	PartitionParticleFilterParameters params_;

	/** Partition sampling */
	template<class Space, class VarFunction, class MotionModel>
	void Track(const Space& space, const VarFunction& function, MotionModel motion) {
		BOOST_ASSERT(UseAnnealing == false);
		typedef BetterMeansBigger<Score> CMP;
		motion.SetSuppressionFactor(params_.noise_suppression_factor_);
		// pin down varying function
		PinnedFunction<unsigned int, State, Score, VarFunction> pinned(function);
		// create initial sample set
		SampleSet open_samples(this->pickMany(space, params_.particle_count_));
		// tracking loop
		for(unsigned int tt=0; this->isRunning(tt); tt++) {
			// set pin down time
			pinned.setTime(tt);
			// only the first time: evaluate initial sample set
			if(tt == 0) {
				open_samples.ComputeLikelihood(pinned);
				//open_samples.printScoresAndWeights(std::cout);
			}
			// create new sample set using weighted random drawing and set score to 1/n
			try{
				open_samples.Resample(params_.particle_count_);
				//open_samples.printScoresAndWeights(std::cout);
			} catch(CanNotNormalizeZeroListException&) {
				LOG_WARNING << "All samples have a score of zero. This means the tracker lost the object!";
				// tracker lost the object
				return;
			}
			// process all partitions one after another
			// TODO branching?
			for(std::vector<Partition>::const_iterator it=params_.partitions_.begin(); it!=params_.partitions_.end(); ++it) {
				std::cout << "Partition " << (it - params_.partitions_.begin()) << " '" << it->name_ << "'" << std::endl;
				// apply motion model for the current partition only by setting the noise of other partitions to 0 (or the suppression factor)
				motion.SetPartition(it->noise_indices_);
				open_samples.TransformStates(motion);
				// perform a weighted resampling (currently using the normal evaluation function)
				open_samples.WeightedResample(pinned); // TODO partial resampling - use partition particle counts
				//open_samples.printScoresAndWeights(std::cout);
				// notify samples
				this->NotifySamples(open_samples);
			}
			// compute likelihood
			open_samples.ComputeLikelihood(pinned);
			//open_samples.printScoresAndWeights(std::cout);
			// tracing
			this->NotifyTimestepResult(tt, this->template take<Space, CMP>(space, open_samples));
		}
	}

	template<class Space, class VarFunction, class MotionModel>
	void TrackOld(const Space& space, const VarFunction& function, MotionModel motion) {
		typedef BetterMeansBigger<Score> CMP;
		// pin down varying function
		PinnedFunction<unsigned int, State, Score, VarFunction> pinned(function);
		// initialize sample set with random samples
		// Remark: We use one sample and the same sample set for all individual optimizations
		//         This way the optimizations are not completely independent, but as the noise
		//         at the beginning should be low it should not matter.
		//         Perhaps it is even better ...
		SampleSet open_samples(this->pickMany(space, params_.particle_count_));
		// tracking loop
		for(unsigned int tt=0; this->isRunning(tt); tt++) {
			// set pin down time
			pinned.setTime(tt);
			// evaluate the initially picked samples
			if(tt == 0) {
				open_samples.EvaluateAll(pinned);
			}
			// several iterations
			for(unsigned int repetition=0; repetition<params_.repetition_count_; repetition++) {
				// one part after the other
				for(std::vector<Partition>::const_iterator it=params_.partitions_.begin(); it!=params_.partitions_.end(); ++it) {
					std::cout << "Partition " << (it - params_.partitions_.begin()) << " '" << it->name_ << "'" << std::endl;
					// restrict noise to partition
					motion.SetPartition(it->noise_indices_, params_.noise_suppression_factor_);
					motion.SetNoiseAmount(1.0f);
					// apply motion model which is simply white noise
					open_samples.TransformStates(motion);
					// run optimization
					if(UseAnnealing) {
						// use annealing to refine the sample set
						ParticleAnnealing<State, Score, TracePolicy::Samples::ForwardToObject<State,Score,NotifySamples> > pa;
						pa.SetNotifySamplesObject(this);
						pa.settings_.layers_ = it->annealing_layers_;
						pa.settings_.particle_count_ = it->particle_count_;
						pa.settings_.alpha_ = it->alpha_;
						pa.OptimizeInplace(open_samples, space, pinned, motion);
					} else {
						// evaluate samples
						open_samples.EvaluateAll(pinned);
						// notify samples
						this->NotifySamples(open_samples);
						// create new sample set using weighted random drawing
						try{
							open_samples.Resample(it->particle_count_);
						} catch(typename SampleSet::CanNotNormalizeZeroListException&) {
							LOG_WARNING << "All samples have a score of zero. This means the tracker lost the object!";
							// tracker lost the object
							return;
						}
					}
				}
			}
			// tracing
			this->NotifyTimestepResult(tt, this->template take<Space, CMP>(space, open_samples));
		}
	}

//	template<class Space, class VarFunction, class MotionModel>
//	Solution Track(const TimeRange& range, const Space& space, const VarFunction& function, MotionModel motion) {
//		return Track(Solution(range), space, function, motion);
//	}

	template<class Space, class VarFunction>
	void TrackWhiteNoise(const Space& space, const VarFunction& function, const std::vector<double>& noise) {
		MotionModels::SpaceRandomMotionModel<Space> motion(space, noise);
		Track(space, function, motion);
	}

//	template<class Space, class VarFunction>
//	Solution TrackWhiteNoise(const TimeRange& range, const Space& space, const VarFunction& function, const std::vector<double>& noise) {
//		MotionModels::SpaceRandomMotionModel<Space> motion(space, noise);
//		return Track(range, space, function, motion);
//	}

};

template<typename State, typename Score, class TimeControl, class StartingStates, class Take, class NotifySamples, class TimestepResult>
struct PartitionParticleFilterWithAnnealing
: public PartitionParticleFilter<State, Score,TimeControl,  StartingStates, Take, NotifySamples, TimestepResult, true>
{};

template<typename State, typename Score, class TimeControl, class StartingStates, class Take, class NotifySamples, class TimestepResult>
struct PartitionParticleFilterNoAnnealing
: public PartitionParticleFilter<State, Score, TimeControl, StartingStates, Take, NotifySamples, TimestepResult, false>
{};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
