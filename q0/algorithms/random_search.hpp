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

		void init(const Domain&) { }
		
		State operator()(const Domain& dom, const State&) {
			return domains::random(dom);
		}
		
		template<typename Compare, typename Score>
		bool test(Compare cmp, Score a, Score b) {
			return cmp(a, b);
		}
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

		template<typename Compare, typename Score>
		bool test(Compare cmp, Score a, Score b) {
			if(!cmp(a, b)) {
				radius_ *= decrease_factor_;
				return false;
			}
			else return true;
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

		template<typename Compare, typename Score>
		bool test(Compare cmp, Score a, Score b) {
			if(!cmp(a, b)) {
				// invert search direction and decrease search radius
				step_length_[k_] *= -p_PatternSearch_factor;
				return false;
			}
			else return true;
		}

	private:
		Tangent zero_;
		std::vector<double> step_length_;
		unsigned int k_;
	};	

	constexpr double p_SimulatedAnnealing_initial_radius = 1.0;
	constexpr double p_SimulatedAnnealing_initial_temperature = 1.0;
	constexpr double p_SimulatedAnnealing_factor = 0.95;

	/** Walks into a random direction and accepts worse particles based on a probability */
	template<typename Domain>
	struct SimulatedAnnealing
	{
		typedef typename domains::state_type<Domain>::type State;
		typedef float T;
		typedef typename domains::tangent_type<T,Domain>::type Tangent;		

		void init(const Domain& dom) {
			radius_ = p_SimulatedAnnealing_initial_radius;
			temperature_ = p_SimulatedAnnealing_initial_temperature;

		}
		
		State operator()(const Domain& dom, const State& x) {
//			std::cout << "r=" << radius_ << ", T=" << temperature_ << std::endl;
			double radius_old = radius_;
			radius_ *= p_SimulatedAnnealing_factor;
			return domains::random_neighbour(dom, x, radius_old);
		}

		template<typename Compare, typename Score>
		bool test(Compare cmp, Score a, Score b) {
			if(temperature_ == 0) {
				// cold as death
				return false;
			}
			double temperature_old = temperature_;
			temperature_ *= p_SimulatedAnnealing_factor;
			double ds = a - b; // FIXME does this work?
			if(ds < 0) {
//				std::cout << a << " is better than " << b << std::endl;
				return true;
			}
			double p = std::exp(-ds/temperature_old);
//			std::cout << a << " / " << b <<  " -> p=" << p;
			if(math::random_uniform<double>(0,1) < p) {
//				std::cout << " YES" << std::endl;
				return true;
			}
			else {
//				std::cout << " NO" << std::endl;
				return false;
			}
		}

	private:
		double radius_;
		double temperature_;
	};	

	constexpr unsigned int p_random_search_N = 45;

	/** Random search */
	template<template<typename> class Method>
	struct random_search
	{
		template<typename Domain, typename Objective, typename Control, typename Compare>
		struct impl
		{
			typedef typename domains::state_type<Domain>::type State;
			typedef typename std::result_of<Objective(State)>::type Score;

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
						if(ops[i].test(cmp, particles_new.scores[i], particles.scores[i])) {
							particles.states[i] = particles_new.states[i];
							particles.scores[i] = particles_new.scores[i];
							// check if this particle is an improvement to the overall best
							if(cmp(particles.scores[i], best.score)) {
								best.state = particles.states[i];
								best.score = particles.scores[i];
							}
						}
					}
				}
				return best;
			}
		};
	};

	/** Random search */
	template<template<typename> class Method>
	struct random_search_1
	{
		template<typename Domain, typename Objective, typename Control, typename Compare>
		struct impl
		{
			typedef typename domains::state_type<Domain>::type State;
			typedef typename std::result_of<Objective(State)>::type Score;

			static inline particle<State,Score> apply(const Domain& dom, Objective f, Control control, Compare cmp) {
				particle<State,Score> best;
				best.state = domains::random(dom);
				best.score = f(best.state);
				Method<Domain> op;
				op.init(dom);
				while(!control(best)) {
					State u = op(dom, best.state);
					Score s = f(u);
					if(op.test(cmp, s, best.score)) {
						best.state = u;
						best.score = s;
					}
				}
				return best;
			}
		};
	};

}

template<typename Domain, typename Objective, typename Control, typename Compare>
struct monte_carlo
: detail::random_search<detail::MonteCarlo>::impl<Domain,Objective,Control,Compare> {};

template<typename Domain, typename Objective, typename Control, typename Compare>
struct local_unimodal_search
: detail::random_search<detail::LocalUnimodalSearch>::impl<Domain,Objective,Control,Compare> {};

template<typename Domain, typename Objective, typename Control, typename Compare>
struct pattern_search
: detail::random_search<detail::PatternSearch>::impl<Domain,Objective,Control,Compare> {};

template<typename Domain, typename Objective, typename Control, typename Compare>
struct simulated_annealing
: detail::random_search<detail::SimulatedAnnealing>::impl<Domain,Objective,Control,Compare> {};

template<typename Domain, typename Objective, typename Control, typename Compare>
struct monte_carlo_1
: detail::random_search_1<detail::MonteCarlo>::impl<Domain,Objective,Control,Compare> {};

template<typename Domain, typename Objective, typename Control, typename Compare>
struct local_unimodal_search_1
: detail::random_search_1<detail::LocalUnimodalSearch>::impl<Domain,Objective,Control,Compare> {};

template<typename Domain, typename Objective, typename Control, typename Compare>
struct pattern_search_1
: detail::random_search_1<detail::PatternSearch>::impl<Domain,Objective,Control,Compare> {};

template<typename Domain, typename Objective, typename Control, typename Compare>
struct simulated_annealing_1
: detail::random_search_1<detail::SimulatedAnnealing>::impl<Domain,Objective,Control,Compare> {};


}}
//---------------------------------------------------------------------------
#endif
