/*
 * PSO.h
 *
 *  Created on: Sep 5, 2010
 *      Author: david
 */

#ifndef QUESTZERO_ALGORITHMS_PSO_H_
#define QUESTZERO_ALGORITHMS_PSO_H_
//---------------------------------------------------------------------------
#include "QuestZero/Common/RandomNumbers.h"
#include "QuestZero/Common/Sample.h"
#include "QuestZero/Common/CombinedSampleList.h"
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

namespace detail
{

	template<typename State, typename Score>
	class GlobalData;

	template<typename State, typename Score>
	struct ParticleData
	{
		typedef State state_t;
		typedef Score score_t;

		ParticleData() {}

		ParticleData(State x) {
			last = x;
			current_state_ = x;
			best_state_ = x;
		}

		State last;

		State current_state_;
		Score current_score_;

		State best_state_;
		Score best_score_;
	};

	template<typename State, typename Score>
	const State& get_state(const ParticleData<State,Score>& x) { return x.current_state_; }

	template<typename State, typename Score>
	State& get_state(ParticleData<State,Score>& x) { return x.current_state_; }

	template<typename State, typename Score>
	const Score& get_score(const ParticleData<State,Score>& x) { return x.current_score_; }

	template<typename State, typename Score>
	Score& get_score(ParticleData<State,Score>& x) { return x.current_score_; }

	template<typename State, typename Score>
	class GlobalData
	{
	public:
		typedef TSample<State,Score> Sample;

		GlobalData()
		: omega_(1.0),
		  psi_personal_(2.05),
		  psi_global_(2.00)
		{}

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

		Score best_score() const { return best_score_; }

		const State& best_state() const { return best_state_; }

		void set(const ParticleData<State,Score>& s) {
			best_state_ = s.current_state_;
			best_score_ = s.current_score_;
		}

		template<class Space>
		void Update(const Space& space, ParticleData<State,Score>& u) {
			// TODO double or float?
			double fl = omega();
			double fp = psi_personal() * RandomNumbers::Uniform<double>();
			double fg = psi_global() * RandomNumbers::Uniform<double>();
			State dl = space.difference(u.current_state_, u.last);
			State dp = space.difference(u.best_state_, u.current_state_);
			State dg = space.difference(best_state(), u.current_state_);
			u.last = u.current_state_;
			State delta = space.weightedSum(fl, dl, fp, dp, fg, dg);
			// FIXME we should bound the "velocity" i.e. the maximal distance made in a step
			u.current_state_ = space.project(space.compose(delta, u.current_state_));
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
		State best_state_;
		Score best_score_;
	};

}

/// <summary>
/// The particle swarm algorithm
/// See http://en.wikipedia.org/wiki/Particle_swarm_optimization
/// See http://www.hvass-labs.org/projects/swarmops/
/// See Particle Swarm Optimization: Developments, Application and Ressources, Eberhart, R. and Shi, Y.
/// </summary>
template<typename State, typename Score,
	typename ExitPolicy,
	typename InitializePolicy
>
struct PSO
: public ExitPolicy,
  public InitializePolicy
{
	typedef detail::ParticleData<State,Score> Particle;
	typedef std::vector<Particle> ParticleSet;

	std::string name() const { return "PSO"; }

	PSOSettings settings;

	template<class Space, class Function, typename Compare, typename Visitor>
	TSample<State,Score> Optimize(const Space& space, const Function& function, Compare cmp, Visitor vis) {
		globals.set(settings);
		// generate start samples
		add_random_samples(particles, *this, space, settings.particleCount);
		// compute particle score
		compute_likelihood(particles, function);
		// initialize personal best
		for(Particle& p : particles) {
			p.best_state_ = p.current_state_;
			p.best_score_ = p.current_score_;
		}
		// initialize global best
		auto best_id = find_best_by_score(particles, cmp);
		globals.set(get_sample(particles, best_id));
		// iterate
		while(true) {
			// evaluate samples
			LOG_DEBUG << "PSO: evaluate states";
			compute_likelihood(particles, function);
			// notify about new samples
			LOG_DEBUG << "PSO: notify new samples";
			vis.NotifySamples(particles);
			// find best sample
			auto best_id = find_best_by_score(particles, cmp);
			// check if break condition is satisfied
			LOG_DEBUG << "PSO: check exit condition";
			if(this->IsTargetReached(globals.best_score(), cmp)) {
				Particle best = get_sample(particles, best_id);
				return { get_state(best), get_score(best) };
			}
			// update global best
			LOG_DEBUG << "PSO: update global best";
			if(cmp(get_score(particles, best_id), globals.best_score())) {
				globals.set(get_sample(particles, best_id));
			}
			// update personal best and particle
			LOG_DEBUG << "PSO: update personal best";
			for(size_t i = 0; i < particles.size(); i++) {
				Particle& p = particles[i];
				if(cmp(p.current_score_, p.best_score_)) {
					p.best_state_ = p.current_state_;
					p.best_score_ = p.current_score_;
				}
				globals.Update(space, p);
			}
		}
	}

private:
	ParticleSet particles;

	detail::GlobalData<State,Score> globals;

};
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
