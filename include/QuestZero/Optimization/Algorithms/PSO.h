/*
 * PSO.h
 *
 *  Created on: Sep 5, 2010
 *      Author: david
 */

#ifndef QUESTZERO_ALGORITHMS_PSO_H_
#define QUESTZERO_ALGORITHMS_PSO_H_
//---------------------------------------------------------------------------
#include "QuestZero/Common/ScoreComparer.h"
#include "QuestZero/Common/RandomNumbers.h"
#include "QuestZero/Common/Sample.h"
#include "QuestZero/Common/SampleSet.h"
#include "QuestZero/Common/Log.h"
#include <vector>
#include <string>
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

struct PSOSettings
{
	PSOSettings() {
		particleCount = 200;
		factor_velocity = 1.0;
		factor_personal = 2.05;
		factor_global = 2.05;
	}
	unsigned int particleCount;
	double factor_velocity;
	double factor_personal;
	double factor_global;
};

/// <summary>
/// The particle swarm algorithm
/// See http://en.wikipedia.org/wiki/Particle_swarm_optimization
/// See http://www.hvass-labs.org/projects/swarmops/
/// See Particle Swarm Optimization: Developments, Application and Ressources, Eberhart, R. and Shi, Y.
/// </summary>
template<
	class State,
	class Score,
	class Target,
	class StartingStates,
	class Take,
	class NotifySamples,
	bool Minimize
>
struct PSO
: public Target,
  public StartingStates,
  public Take,
  public NotifySamples
{
	typedef TSample<State,Score> Sample;
	typedef TSampleSet<State,Score> SampleSet;

	PSO() {}

	virtual ~PSO() {}

	std::string name() const { return "PSO"; }

	PSOSettings settings;

	typedef typename ComparerSelector<Score,Minimize>::Result CMP;

	template<class Space, class Function>
	Sample Optimize(const Space& space, const Function& function) {
		globals.set(settings);
		// generate start samples
		std::vector<State> initial_states = this->pickMany(space, settings.particleCount);
		particles.reserve(initial_states.size());
		for(typename std::vector<State>::const_iterator it=initial_states.begin(); it!=initial_states.end(); ++it) {
			//LOG_DEBUG << "PSO: Picked initial state: " << *it;
			particles.push_back(ParticleData(*it));
		}
		// iterate
		while(true) {
			// construct sample set
			LOG_DEBUG << "PSO: getting current states";
			SampleSet samples = currentSamples();
			// evaluate samples
			LOG_DEBUG << "PSO: evaluate states";
			compute_likelihood(samples, function);
			// update global best
			LOG_DEBUG << "PSO: update global best";
			auto best_id = find_best_by_score(samples, CMP());
			if(!globals.isSet() || CMP::compare(get_score(samples, best_id), globals.best_score())) {
				globals.set(get_sample(samples, best_id));
			}
			// update personal best and particle
			LOG_DEBUG << "PSO: update personal best and particle";
			for(size_t i = 0; i < particles.size(); i++) {
				Score s = get_score(samples, i);
				ParticleData& p = particles[i];
				if(CMP::compare(s, p.best_score_)) {
					p.best_state_ = get_state(samples, i);
					p.best_score_ = s;
				}
				p.Update(space, globals);
			}
			// notify about new samples
			LOG_DEBUG << "PSO: notify about new samples";
			this->NotifySamples(bestSamples());
			// check if break condition is satisfied
			LOG_DEBUG << "PSO: check break condition";
			if(this->IsTargetReached(globals.best_score())) {
				break;
			}
		}
		LOG_DEBUG << "PSO: picking result";
		return this->template take<Space, CMP>(space, bestSamples());
	}

private:
	class GlobalData
	{
	public:
		GlobalData()
		: is_best_set(false) {}

		void set(const PSOSettings& x) {
			omega_ = x.factor_velocity;
			psi_personal_ = x.factor_personal;
			psi_global_ = x.factor_global;
			double k = ComputeK();
			omega_ *= k;
			psi_personal_ *= k;
			psi_global_ *= k;
			best_score_ = 1e12;
		}

		double omega() const { return omega_; }

		double psi_personal() const { return psi_personal_; }

		double psi_global() const { return psi_global_; }

		bool isSet() const { return is_best_set; }

		Score best_score() const { return best_score_; }

		const State& best_state() const { return best_state_; }

		void set(const Sample& s) {
			best_state_ = s.state;
			best_score_ = s.score;
			is_best_set = true;
		}

	private:
		/** Construction factor (see Clerc, 1999) */
		double ComputeK() {
			double u = psi_personal_ + psi_global_;
			if(u <= 4) {
				throw std::runtime_error("psi_personal + psi_global must be > 4!");
			}
			return 2.0 / std::abs(2.0 - u - sqrt(u * u - 4.0 * u));
		}

		double omega_;
		double psi_personal_;
		double psi_global_;
		bool is_best_set;
		State best_state_;
		Score best_score_;
	};

	struct ParticleData
	{
		ParticleData(State x) {
			last = x;
			current = x;
			best_state_ = x;
			best_score_ = 1e12;
		}

		State last;
		State current;
		State best_state_;
		Score best_score_;

		template<class Space>
		void Update(const Space& space, const GlobalData& globals) {
			// TODO double or float?
			double fl = globals.omega();
			double fp = globals.psi_personal() * RandomNumbers::Uniform<double>();
			double fg = globals.psi_global() * RandomNumbers::Uniform<double>();
			State dl = space.difference(current, last);
			State dp = space.difference(best_state_, current);
			State dg = space.difference(globals.best_state(), current);
			last = current;
			State delta = space.weightedSum(fl, dl, fp, dp, fg, dg);
			// FIXME we should bound the "velocity" i.e. the maximal distance made in a step
			current = space.project(space.compose(delta, current));
		}
	};

private:
	std::vector<ParticleData> particles;

	GlobalData globals;

	std::vector<State> currentStates() const {
		std::vector<State> states;
		states.reserve(particles.size());
		for(typename std::vector<ParticleData>::const_iterator it=particles.begin(); it!=particles.end(); ++it) {
			states.push_back(it->current);
		}
		return states;
	}

	SampleSet currentSamples() const {
		return SampleSet(currentStates());
	}

	SampleSet bestSamples() const {
		SampleSet samples;
		give_size_hint(samples, particles.size());
		for(typename std::vector<ParticleData>::const_iterator it=particles.begin(); it!=particles.end(); ++it) {
			add_sample(samples, it->best_state_, it->best_score_);
		}
		return samples;
	}

};

//---------------------------------------------------------------------------

template< class State, class Score, class Target, class StartingStates, class Take, class NotifySamples>
struct PSO_min : public PSO<State, Score, Target, StartingStates, Take, NotifySamples, true> {};

template< class State, class Score, class Target, class StartingStates, class Take, class NotifySamples>
struct PSO_max : public PSO<State, Score, Target, StartingStates, Take, NotifySamples, false> {};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
