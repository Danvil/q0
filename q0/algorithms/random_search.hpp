#ifndef Q0_ALGORITHMS_RANDOMSEARCH_HPP_
#define Q0_ALGORITHMS_RANDOMSEARCH_HPP_
#include <q0/q0.hpp>
#include <q0/common.hpp>
#include <q0/domains.hpp>
#include <q0/objective.hpp>
#include <q0/particle_vector.hpp>
#include <q0/math.hpp>
#include <cmath>
//---------------------------------------------------------------------------
namespace q0 { namespace algorithms {

namespace detail
{
	/** Picks totally random state */
	template<typename Domain>
	struct MonteCarlo
	{
		typedef typename domains::state_type<Domain>::type State;
		void init(const Domain&) {}
		State operator()(const Domain& dom, const State&) {
			return domains::random(dom);
		}
		void success() { }
		void failure() { }
	};

	constexpr double p_RandomWalk_radius = 0.1;

	/** Picks random state in neighbourhood */
	template<typename Domain>
	struct RandomWalk
	{
		typedef typename domains::state_type<Domain>::type State;
		void init(const Domain& dom) {}
		State operator()(const Domain& dom, const State& x) {
			return domains::random_neighbour(dom, x, p_RandomWalk_radius);
		}
		void success() { }
		void failure() { }
	};

	constexpr double p_LocalUnimodalSearch_initial_radius = 1.0;
	constexpr double p_LocalUnimodalSearch_gamma = 3.0;

	/** Picks random state in neighbourhood and decreases range on failure */
	template<typename Domain>
	struct LocalUnimodalSearch
	{
		typedef typename domains::state_type<Domain>::type State;
		
		void init(const Domain& dom) {
			radius_ = p_LocalUnimodalSearch_initial_radius;
			unsigned int dim = domains::dimension(dom);
			decrease_factor_ = std::pow(0.5, 1.0 / (p_LocalUnimodalSearch_gamma * static_cast<double>(dim)));
		}

		State operator()(const Domain& dom, const State& x) {
			return domains::random_neighbour(dom, x, radius_);
		}

		void success() { }

		void failure() {
			radius_ *= decrease_factor_;
		}

	private:
		double radius_;
		double decrease_factor_;
	};

	constexpr double p_PatternSearch_initial_radius = 1.0;
	constexpr double p_PatternSearch_factor = 0.5;

	/** Walks into a random direction and decreases step size on failure */
	template<typename Domain>
	struct PatternSearch
	{
		typedef typename domains::state_type<Domain>::type State;
		typedef float T;
		typedef typename domains::tangent_type<T,Domain>::type Tangent;		

		void init(const Domain& dom) {
			const unsigned int dim = domains::dimension(dom);
			step_length_.resize(dim, p_PatternSearch_initial_radius);
			zero_ = Tangent(dim);
			for(unsigned int i=0; i<dim; i++) {
				at(zero_,i) = 0;
			}
		}
		
		State operator()(const Domain& dom, const State& x) {
			Tangent t = zero_;
			k_ = math::random_int<unsigned int>(0, t.size()-1);
			at(t,k_) = step_length_[k_];
			return domains::exp<T>(dom, x, t);
		}

		void success() { }

		void failure() {
			// invert search direction and decrease search radius
			step_length_[k_] *= -p_PatternSearch_factor;
		}

	private:
		Tangent zero_;
		std::vector<double> step_length_;
		unsigned int k_;
	};	

}

constexpr unsigned int p_random_search_N = 100;

/** Random search */
template<template<typename> class Method>
struct random_search
{
	template<typename Domain, typename Objective, typename Control, typename Compare>
	struct impl
	{
		typedef typename domains::state_type<Domain>::type State;
		typedef typename objective::argument_type<Objective>::type State2;
		// FIXME assert that State==State2
		typedef typename objective::result_type<Objective>::type Score;

		static inline particle<State,Score> apply(const Domain& dom, Objective f, Control control, Compare cmp) {
			particle_vector<State,Score> particles;
			particles.set_states(domains::random(dom, p_random_search_N));
			particles.evaluate(f);
			particle<State,Score> best = particles.find_best(cmp);
			std::vector<Method<Domain>> ops(p_random_search_N);
			for(std::size_t i=0; i<particles.size(); i++) {
				ops[i].init(dom);
			}
			while(!control(particles, best)) {
				// create next round of particles
				particle_vector<State,Score> particles_new;
				particles_new.resize(particles.size());
				for(std::size_t i=0; i<particles.size(); i++) {
					particles_new.states[i] = ops[i](dom, particles.states[i]);
				}
				// evaluate particles
				particles_new.evaluate(f);
				// check if new particles are an improvement
				for(std::size_t i=0; i<particles.size(); i++) {
					if(cmp(particles_new.scores[i], particles.scores[i])) {
						ops[i].success();
						particles.states[i] = particles_new.states[i];
						particles.scores[i] = particles_new.scores[i];
						// check if this particle is an improvement to the overall best
						if(cmp(particles.scores[i], best.score)) {
							best.state = particles.states[i];
							best.score = particles.scores[i];
						}
					}
					else {
						ops[i].failure();
					}
				}
			}
			return best;
		}
	};
};

}}
//---------------------------------------------------------------------------
#endif
