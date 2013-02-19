#ifndef Q0_OBJECTIVE_HPP_
#define Q0_OBJECTIVE_HPP_
#include "common.hpp"
#include "domains.hpp"
#include <vector>
#include <type_traits>
//---------------------------------------------------------------------------
namespace q0 {

template<typename Domain, typename Objective>
struct problem_traits
{
	typedef typename domains::state_type<Domain>::type state_t;
	typedef typename std::result_of<Objective(state_t)>::type score_t;
	typedef particle<state_t, score_t> particle_t;
};

namespace objective {

// template<typename Domain, typename Objective>
// struct result_of {
// 	typedef typename domains::state_type<Domain>::type state_t;
// 	typedef typename std::result_of<Objective(state_t)>::type type;
// };

template<typename Objective, typename State>
std::vector<typename std::result_of<Objective(State)>::type> parallel(
	Objective f, const std::vector<State>& states) {
	std::vector<typename std::result_of<Objective(State)>::type> scores(states.size());
	for(std::size_t i=0; i<states.size(); i++) {
		scores[i] = f(states[i]);
	}
	return scores;
}

}}
//---------------------------------------------------------------------------
#endif
