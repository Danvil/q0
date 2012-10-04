#ifndef Q0_ALGORITHMS_RANDOMSEARCH_HPP_
#define Q0_ALGORITHMS_RANDOMSEARCH_HPP_
#include <q0/q0.hpp>
#include <q0/common.hpp>
#include <q0/domains.hpp>
#include <q0/objective.hpp>
#include <q0/tools.hpp>
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

	static inline particle<State,Score> apply(const Domain& domain, Objective f, Control control, Compare cmp) {
		std::vector<State> states = domains::random(domain, N);
		std::vector<Score> scores = objective::parallel(f, states);
		particle<State,Score> best = tools::find_best(states, scores, cmp);
		while(!control(best.state, best.score)) {
			states = domains::random(domain, N);
			scores = objective::parallel(f, states);
			particle<State,Score> best_cur = tools::find_best(states, scores, cmp);
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
