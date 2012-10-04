#ifndef Q0_PARTICLEVECTOR_HPP_
#define Q0_PARTICLEVECTOR_HPP_
#include <q0/common.hpp>
#include <q0/objective.hpp>
#include <boost/assert.hpp>
#include <vector>
#include <algorithm>
//---------------------------------------------------------------------------
namespace q0 {

template<typename State, typename Score>
struct particle_vector
{
	std::vector<State> states;
	std::vector<Score> scores;

	std::size_t size() const {
		BOOST_ASSERT(states.size() == scores.size());
		return states.size();
	}

	template<typename Compare>
	particle<State,Score> find_best(Compare cmp) const {
		BOOST_ASSERT(states.size() == scores.size());
		BOOST_ASSERT(states.size() > 0);
		std::size_t i = std::distance(scores.begin(), std::min_element(scores.begin(), scores.end(), cmp));
		return {states[i], scores[i]};
	}

	template<typename F>
	void evaluate(F f) {
		scores = objective::parallel(f, states);
	}

	void set_states(const std::vector<State>& u) {
		states = u;
	}

};

}
//---------------------------------------------------------------------------
#endif
