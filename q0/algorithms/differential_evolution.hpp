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

/** Random search */
struct differential_evolution
{
	struct Parameters
	{
		unsigned int N;
		double F;
		double CR;

		Parameters() :
			N(45),
			F(0.5),
			CR(0.1)
		{}
	};

	Parameters parameters;

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

	template<typename Domain, typename Objective, typename Control, typename Compare>
	typename problem_traits<Domain,Objective>::particle_t
	solve(const Domain& dom, Objective f, Control control, Compare cmp)
	{
		typedef typename domains::state_type<Domain>::type State;
		typedef typename domains::scalar_type<Domain>::type Scalar;
		typedef typename std::result_of<Objective(State)>::type Score;

		// initial particles
		particle_vector<State,Score> particles;
		particles.set_states(domains::random(dom, parameters.N));
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
				auto d = static_cast<Scalar>(parameters.F)*domains::log(dom, particles.states[indices[1]], particles.states[indices[2]]);
				auto m = domains::exp(dom, particles.states[indices[0]], d);
				auto x = domains::log(dom, particles.states[i], m);
				// crossover
				std::size_t k = math::random_int<std::size_t>(0, dim-1);
				for(std::size_t j=0; j<dim; j++) {
				 	if(j != k && math::random_uniform<Scalar>(0,1) > parameters.CR) {
				 		at(x,j) = 0;
				 	}
				}
				// store for parallel evaluation and selection
				particles_new.states[i] = domains::exp(dom, particles.states[i], x);
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
