#ifndef Q0_ALGORITHMS_RANDOMSEARCH_HPP_
#define Q0_ALGORITHMS_RANDOMSEARCH_HPP_
#include <q0/q0.hpp>
#include <q0/common.hpp>
#include <q0/domains.hpp>
#include <q0/objective.hpp>
#include <q0/particle_vector.hpp>
//---------------------------------------------------------------------------
namespace q0 { namespace algorithms {

/** Random search */
template<typename Domain, typename Objective, typename Control, typename Compare>
struct random_search
{
	typedef typename domains::state_type<Domain>::type State;
	typedef typename objective::argument_type<Objective>::type State2;
	// FIXME assert that State==State2
	typedef typename objective::result_type<Objective>::type Score;

	static constexpr unsigned int N = 100;

	static inline particle<State,Score> apply(const Domain& dom, Objective f, Control control, Compare cmp) {
		particle_vector<State,Score> particles;
		particles.set_states(domains::random(dom, N));
		particles.evaluate(f);
		particle<State,Score> best = particles.find_best(cmp);
		while(!control(best.state, best.score)) {
			particles.set_states(domains::random(dom, N));
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
