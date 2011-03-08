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
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

//namespace Private
//{
//	template<class T, class X>
//	struct CheckForMultiplierState;
//
//	template<class T, class X>
//	struct CheckForMultiplierState<T, Spaces::MultiplierState<X> > {};
//
//	template<class T, class X>
//	struct CheckForMultiplierSpace;
//
//	template<class T, class X>
//	struct CheckForMultiplierSpace<T, Spaces::MultiplierSpace<X> > {};
//
//}

struct AlternatingParticleFilterParameters
{
	AlternatingParticleFilterParameters() {
		particle_count_ = 1000;
		repetition_count_ = 2;
		noise_supression_factor_ = 0.1;
		annealing_layers_ = 10;
	}

	unsigned long particle_count_;

	unsigned int repetition_count_;

	double noise_supression_factor_;

	unsigned int annealing_layers_;

	std::vector<double> noise_;

};

/**
 * FIXME assert that State==MultiplierState<BaseState>
 * FIXME assert that Space==MultiplierSpace<BaseSpace,MultiplierState<BaseState> >
 * Function takes a MultiplierState<BaseState>!
 * Samples consists of MultiplierState<BaseState>! (which is a waste)
 * Solution consists of MultiplierState<BaseState>!
 *
 * Total number of evaluations per time step:
 * 	# = RepetitionCount * StateDimension * AnnealingLayerCount * ParticleCount
 */
template<typename Time, typename State, typename Score, class StartingStates, class Take, class NotifySamples, class NotifySolution, bool UseAnnealing>
struct AlternatingParticleFilter
: public StartingStates,
  public Take,
  public NotifySamples,
  public NotifySolution
{
	typedef TSolution<Time, State, Score> Solution;
	typedef TTimeRange<Time> TimeRange;
	typedef TSampleSet<State, Score> SampleSet;

	AlternatingParticleFilterParameters params_;

	template<class Space, class VarFunction>
	Solution Track(const TimeRange& range, const Space& space, const VarFunction& function) {
		typedef BetterMeansBigger<Score> CMP;
		// prepare the solution
		Solution sol(range);
		// pin down varying function
		PinnedFunction<Time, State, Score, VarFunction> pinned(function, range.begin());
		// prepare object noise
		// Remark: The trick is to adapt the noise vector such that only the current
		//         object is changed. We provide the option to not set the noise
		// 		   to zero but to only scale it down.
		std::vector<std::vector<double> > part_noise_list(space.template count());
		for(size_t obj=0; obj<part_noise_list.size(); obj++) {
			std::vector<double> noise = params_.noise_;
			size_t obj_noise_pos = space.template GetSummedDimension(obj);
			size_t obj_noise_length = space.template GetPartDimension(obj);
			// set noise for other parts to 0
			for(size_t i=0; i<noise.size(); i++) {
				if(i < obj_noise_pos || obj_noise_pos+obj_noise_length <= i) {
					noise[i] *= params_.noise_supression_factor_;
				}
			}
			part_noise_list[obj] = noise;
		}
		// initialize sample set with random samples
		// Remark: We use one sample and the same sample set for all individual optimizations
		//         This way the optimizations are not completely independent, but as the noise
		//         at the beginning should be low it should not matter.
		//         Perhaps it is even better ...
		SampleSet open_samples(this->pickMany(space, params_.particle_count_));
		// tracking loop
		for(Time t = range.begin(); t != range.end(); ++t) {
			// set pin down time
			pinned.setTime(t);
			// several iterations
			for(unsigned int repetition=0; repetition<params_.repetition_count_; repetition++) {
				// one part after the other
				for(unsigned int object=0; object<space.template count(); object++) {
					// get the beforehand prepared object noise vector
					const std::vector<double>& part_noise = part_noise_list[object];
					// run optimization
					if(UseAnnealing) {
						// use annealing to refine the sample set
						ParticleAnnealing<State, Score, TracePolicy::Samples::ForwardToObject<State,Score,NotifySamples> > pa;
						pa.SetNotifySamplesObject(this);
						pa.settings_.noise_ = part_noise;
						pa.settings_.layers_ = params_.annealing_layers_;
						pa.OptimizeInplace(open_samples, space, pinned);
					} else {
						// apply motion model which is simply white noise
						open_samples.addNoise(space, part_noise);
						// evaluate samples
						open_samples.evaluateAll(pinned);
						// create new sample set using weighted random drawing
						try{
							open_samples = open_samples.DrawByScore(params_.particle_count_);
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
			sol.set(t, this->template take<Space, CMP>(space, open_samples));
			// tracing
			this->NotifySamples(open_samples);
			this->NotifySolution(sol);
		}
		return sol;
	}

};

template<typename Time, typename State, typename Score, class StartingStates, class Take, class NotifySamples, class NotifySolution>
struct AlternatingParticleFilterWithAnnealing
: public AlternatingParticleFilter<Time, State, Score, StartingStates, Take, NotifySamples, NotifySolution, true>
{};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
