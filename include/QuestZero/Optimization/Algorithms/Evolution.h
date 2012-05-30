/*
 * Evolution.h
 *
 *  Created on: 07.07.2011
 *      Author: david
 */

#ifndef ALGORITHMS_EVOLUTION_H
#define ALGORITHMS_EVOLUTION_H
//---------------------------------------------------------------------------
#include "QuestZero/Common/ScoreComparer.h"
#include "QuestZero/Common/Sample.h"
#include "QuestZero/Common/SampleSet.h"
#include <Danvil/Tools/MoreMath.h>
#include <string>
#include <vector>
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

/** An evolutionary optimization algorithm
 * Each round a given amount of particles are exchanges
 * The probable worst particles are removed and new particles are added
 * by reproducing the probable best particles.
 * Reproduction is done in several ways:
 *  1) adding random noise to a particle (VARY)
 *  2) adding a high amount of random noise to a particle (VARY_HIGH)
 *  3) interpolation between two particles (MUTATE)
 */
template<
	typename State,
	class Score,
	class Target,
	class StartingStates,
	class Take,
	class NotifySamples,
	bool Minimize
>
struct Evolution
: public Target,
  public StartingStates,
  public Take,
  public NotifySamples
{
	typedef TSample<State,Score> Sample;
	typedef TSampleSet<State,Score> SampleSet;
	typedef typename ComparerSelector<Score,Minimize>::Result CMP;

	Evolution() {
		agent_count_ = 1024;
		p_mutate_ = 0.2f;
		p_vary_low_ = 0.3f;
		p_vary_high_ = 0.1f;
		noise_low_factor_ = 1.0f;
		noise_high_factor_ = 5.0f;
	}

	virtual ~Evolution() {}

	std::string name() const { return "RND"; }

	unsigned int agent_count_;
	float p_mutate_;
	float p_vary_low_;
	float p_vary_high_;
	float noise_low_factor_;
	float noise_high_factor_;
	std::vector<double> noise_;

	template<class Space, class Function>
	Sample Optimize(const Space& space, const Function& function) {
		// preparations
		int mutate_count = int(p_mutate_ * float(agent_count_));
		int vary_low_count = int(p_vary_low_ * float(agent_count_));
		int vary_high_count = int(p_vary_high_ * float(agent_count_));
		int no_change = agent_count_ - mutate_count - vary_low_count - vary_high_count;
		assert(no_change >= 0);
		std::vector<double> noise_low = noise_;
		std::vector<double> noise_high = noise_;
		for(size_t i=0; i<noise_.size(); i++) {
			noise_low[i] *= noise_low_factor_;
			noise_high[i] *= noise_high_factor_;
		}

		// get the initial set of samples
		SampleSet open(this->template pickMany(space, agent_count_));
		// evaluate initial samples
		open.SetAllWeights(1.0f);
		open.ComputeLikelihood(function);
		// update progress bar
		this->NotifySamples(open);

		// in every iteration add new particles, delete the worst particles
		// iterate until a given condition is fulfilled
		while(!this->IsTargetReached(open.template FindBestScore<CMP>())) {
			// generate new sample set
			SampleSet new_samples;
			new_samples.Reserve(agent_count_);
			// pick probability of old samples
			std::vector<double> density = open.ComputeScoreDensity();
			// low vary
			for(unsigned int i=0; i<(unsigned int)(vary_low_count); i++) {
				size_t p = SamplingTools::SampleDensity(density);
				State state = space.random(open.state(p), noise_low);
				new_samples.Add(state);
			}
			// high vary
			for(unsigned int i=0; i<(unsigned int)(vary_high_count); i++) {
				size_t p = SamplingTools::SampleDensity(density);
				State state = space.random(open.state(p), noise_high);
				new_samples.Add(state);
			}
			// mutate
			for(unsigned int i=0; i<(unsigned int)(mutate_count); i++) {
				size_t p1 = SamplingTools::SampleDensity(density);
				size_t p2 = SamplingTools::SampleDensity(density);
				State state = space.weightedSum(0.5f, open.state(p1), 0.5f, open.state(p2));
				new_samples.Add(state);
			}
			// no change
			for(unsigned int i=0; i<(unsigned int)(no_change); i++) {
				size_t p = SamplingTools::SampleDensity(density);
				const State& state = open.state(p);
				new_samples.Add(state);
			}
			assert(new_samples.Size() == agent_count_);

			// use samples as new sample set and evaluate
			open = new_samples;
			open.SetAllWeights(1.0f);
			open.ComputeLikelihood(function);
			open.printScores(std::cout);

			// update progress bar
			this->NotifySamples(open);
		}
		// return last best samples
		return this->template take<Space, CMP>(space, open);
	}
};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
