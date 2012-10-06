#ifndef Q0_ALGORITHMS_APSO_HPP_
#define Q0_ALGORITHMS_APSO_HPP_
#include <q0/common.hpp>
#include <q0/domains.hpp>
#include <q0/objective.hpp>
#include <q0/particle_vector.hpp>
//---------------------------------------------------------------------------
namespace q0 { namespace algorithms {

constexpr unsigned int apso_N = 45;
constexpr double apso_alpha = 0.3;
constexpr double apso_beta = 0.7;

/** Random search */
template<typename Domain, typename Objective, typename Control, typename Compare>
struct apso
{
	typedef typename domains::state_type<Domain>::type State;
	typedef typename objective::argument_type<Objective>::type State2;
	// FIXME assert that State==State2
	typedef typename objective::result_type<Objective>::type Score;

	static inline particle<State,Score> apply(const Domain& dom, Objective f, Control control, Compare cmp) {
		particle_vector<State,Score> particles;
		particles.set_states(domains::random(dom, apso_N));
		particles.evaluate(f);
		particle<State,Score> best = particles.find_best(cmp);
		while(!control(particles, best)) {
			for(State& x : particles.states) {
				x = domains::lerp(dom, apso_beta, x,
					domains::random_neighbour(dom, best.state, apso_alpha));
			}
			particles.evaluate(f);
			particle<State,Score> best_cur = particles.find_best(cmp);
			if(cmp(best_cur.score, best.score)) {
				best = best_cur;
			}
		}
		return best;
	}
};

}}
//---------------------------------------------------------------------------
#endif
