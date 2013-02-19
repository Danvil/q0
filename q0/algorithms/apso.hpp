#ifndef Q0_ALGORITHMS_APSO_HPP_
#define Q0_ALGORITHMS_APSO_HPP_
#include <q0/common.hpp>
#include <q0/domains.hpp>
#include <q0/objective.hpp>
#include <q0/particle_vector.hpp>
//---------------------------------------------------------------------------
namespace q0 { namespace algorithms {

/** Random search */
struct apso
{
	struct Parameters
	{
		unsigned int num;
		double alpha;
		double beta;

		Parameters()
		: num(45), alpha(0.3), beta(0.3) {}
	};

	Parameters parameters;

	template<typename Domain, typename Objective, typename Control, typename Compare>
	typename problem_traits<Domain,Objective>::particle_t
	solve(const Domain& dom, Objective f, Control control, Compare cmp)
	{
		typedef problem_traits<Domain,Objective> traits;
		typedef typename traits::state_t State;
		typedef typename traits::score_t Score;

		particle_vector<State,Score> particles;
		particles.set_states(domains::random(dom, parameters.num));
		particles.evaluate(f);
		particle<State,Score> best = particles.find_best(cmp);
		while(!control(particles, best)) {
			for(State& x : particles.states) {
				x = domains::lerp(dom, parameters.beta, x,
					domains::random_neighbour(dom, best.state, parameters.alpha));
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
