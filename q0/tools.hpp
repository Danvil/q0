#ifndef Q0_TOOLS_HPP_
#define Q0_TOOLS_HPP_
#include <q0/common.hpp>
#include <boost/random.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <vector>
#include <algorithm>
//---------------------------------------------------------------------------
namespace q0 { namespace tools {

template<typename K>
K random(K a, K b) {
	static boost::random::mt19937 rng;
	boost::random::uniform_real_distribution<K> dist(a,b);
	return dist(rng);
}

template<typename State, typename Score, typename Compare>
particle<State,Score> find_best(const std::vector<State>& states, const std::vector<Score>& scores, Compare cmp) {
	if(states.size() != scores.size()) {
		// FIXME error
	}
	if(states.size() == 0) {
		// FIXME error
	}
	std::size_t i = std::distance(scores.begin(), std::min_element(scores.begin(), scores.end(), cmp));
	return {states[i], scores[i]};
}

}}
//---------------------------------------------------------------------------
#endif
