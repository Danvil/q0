#ifndef Q0_ALGORITHMS_DIFFERENTIALEVOLUTION_HPP_
#define Q0_ALGORITHMS_DIFFERENTIALEVOLUTION_HPP_
#include <q0/q0.hpp>
#include <q0/common.hpp>
#include <q0/domains.hpp>
#include <q0/objective.hpp>
#include <q0/math.hpp>
#include <q0/particle_vector.hpp>
#include <array>
#include <boost/assert.hpp>
//---------------------------------------------------------------------------
namespace q0 { namespace algorithms {

static constexpr unsigned int differential_evolution_N = 45;
static constexpr double differential_evolution_F = 0.5;
static constexpr double differential_evolution_CR = 0.1;

/** Random search */
template<typename Domain, typename Objective, typename Control, typename Compare>
struct differential_evolution
{
	typedef typename domains::state_type<Domain>::type State;
	typedef typename objective::argument_type<Objective>::type State2;
	// FIXME assert that State==State2
	typedef typename objective::result_type<Objective>::type Score;

	typedef float T;

	/** Picks three different indices in {0,...,n-1} \ i */
	template<unsigned int C>
	static std::array<std::size_t,C> pick_mutation_indices(std::size_t i, std::size_t n) {
		// FIXME faster!
		BOOST_ASSERT(n > C);
		std::array<std::size_t,C> result;
		for(unsigned int i=0; i<C; i++) {
			while(true) {
				std::size_t r = math::random_int<std::size_t>(0, n-1);
//				std::cout << r << " - " << std::endl;
				bool ok = (r != i);
				for(unsigned int j=0; j<i && ok; j++) {
					ok = (r != result[j]);
				}
				if(ok) {
					result[i] = r;
					break;
				}
			}
//			std::cout << " -> " << result[i] << std::endl;
		}
//		std::cout << "Finished" << std::endl;
		return result;
	}

	static inline particle<State,Score> apply(const Domain& dom, Objective f, Control control, Compare cmp) {
		// initial particles
		particle_vector<State,Score> particles;
		particles.set_states(domains::random(dom, differential_evolution_N));
		particles.evaluate(f);
		particle<State,Score> best = particles.find_best(cmp);
		const std::size_t n = particles.size();
		const unsigned int dim = dimension(dom);
		while(!control(particles, best)) {
			// compute new candidates through mutation and crossover
			particle_vector<State,Score> particles_new;
			particles_new.resize(n);
			for(std::size_t i=0; i<n; i++) {
				// mutation
				std::array<std::size_t,3> indices = pick_mutation_indices<3>(i, n);
				auto d = differential_evolution_F*domains::log<T>(dom, particles.states[indices[1]], particles.states[indices[2]]);
				auto m = domains::exp<T>(dom, particles.states[indices[0]], d);
				auto x = domains::log<T>(dom, particles.states[i], m);
				// crossover
				std::size_t k = math::random_int<std::size_t>(0, n-1);
				for(std::size_t j=0; j<dim; j++) {
				 	if(j != k && math::random_uniform(0.f,1.f) > differential_evolution_CR) {
				 		at(x,j) = 0;
				 	}
				}
				// store for parallel evaluation and selection
				particles_new.states[i] = domains::exp<T>(dom, particles.states[i], x);
			}
			// mass selection
			particles_new.evaluate(f);
			for(std::size_t i=0; i<n; i++) {
				if(cmp(particles_new.scores[i], particles.scores[i])) {
					particles.states[i] = particles_new.states[i];
					particles.scores[i] = particles_new.scores[i];
					// check if better than best
					if(cmp(particles.scores[i], best.score)) {
						best = particles.get_particle(i);
					}
				}
			}
		}
		return best;
	}
};

}}
//---------------------------------------------------------------------------
#endif
