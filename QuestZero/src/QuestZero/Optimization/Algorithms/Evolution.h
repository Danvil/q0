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
		agent_count_ = 2048;
		exchange_count_ = 256;
		p_mutate_ = 0.3f;
		p_vary_high_ = 0.1f;
		noise_low_factor_ = 1.0f;
		noise_high_factor_ = 5.0f;
	}

	virtual ~Evolution() {}

	std::string name() const { return "RND"; }

	unsigned int agent_count_;
	unsigned int exchange_count_;
	float p_mutate_;
	float p_vary_high_;
	float noise_low_factor_;
	float noise_high_factor_;
	std::vector<double> noise_;

	template<class Space, class Function>
	Sample Optimize(const Space& space, const Function& function) {
		// preparations
		unsigned int mutate_count = (unsigned int)(p_mutate_ * float(exchange_count_));
		unsigned int vary_high_count = (unsigned int)(p_vary_high_ * float(exchange_count_));
		unsigned int vary_low_count = exchange_count_ - mutate_count - vary_high_count;
		std::vector<double> noise_low = noise_;
		std::vector<double> noise_high = noise_;
		for(size_t i=0; i<noise_.size(); i++) {
			noise_low[i] *= noise_low_factor_;
			noise_high[i] *= noise_high_factor_;
		}

		// get the initial set of samples
		SampleSet open(this->template pickMany(space, agent_count_));
		// evaluate initial samples
		open.EvaluateAll(function);
		// update progress bar
		this->NotifySamples(open);

		// in every iteration add new particles, delete the worst particles
		// iterate until a given condition is fulfilled
		while(!this->IsTargetReached(open.template FindBestScore<CMP>())) {
			// generate new samples
			SampleSet new_samples;
			new_samples.Reserve(exchange_count_);
			std::vector<double> density = open.ComputeScoreDensity();
			// low vary
			for(unsigned int i=0; i<vary_low_count; i++) {
				size_t p = open.RandomPickFromDensity(density);
				State state = space.random(open.state(p), noise_low);
				new_samples.Add(state);
			}
			// low vary
			for(unsigned int i=0; i<vary_high_count; i++) {
				size_t p = open.RandomPickFromDensity(density);
				State state = space.random(open.state(p), noise_high);
				new_samples.Add(state);
			}
			// mutate
			for(unsigned int i=0; i<mutate_count; i++) {
				size_t p1 = open.RandomPickFromDensity(density);
				size_t p2 = open.RandomPickFromDensity(density);
				State state = space.weightedSum(0.5f, open.state(p1), 0.5f, open.state(p2));
				new_samples.Add(state);
			}
			assert(new_samples.Size() == exchange_count_);

			// evaluate new samples
			new_samples.EvaluateAll(function);

			// remove worst particles
			assert(open.Size() == agent_count_);
			open = open.template DrawByScore(agent_count_ - exchange_count_);

			// add new samples
			open.Add(new_samples);
			assert(open.Size() == agent_count_);

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
