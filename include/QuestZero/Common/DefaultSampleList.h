/*
 * DefaultTSampleSet<State,Score>.h
 *
 *  Created on: May 31, 2012
 *      Author: david
 */

#ifndef Q0_DEFAULTSAMPLELIST_H_
#define Q0_DEFAULTSAMPLELIST_H_

#include "range.hpp"
#include <boost/assert.hpp>
#include <vector>

namespace Q0 {

// a default data structure for a state list

template<typename State, typename Score>
struct TSampleSet
{
	typedef State state_t;
	typedef Score score_t;
	typedef std::size_t sample_descriptor;

	typedef typename std::vector<state_t>::iterator state_iterator;
	typedef typename std::vector<state_t>::const_iterator state_const_iterator;
	typedef typename std::vector<score_t>::iterator score_iterator;
	typedef typename std::vector<score_t>::const_iterator score_const_iterator;

	std::vector<state_t> states;
	std::vector<score_t> scores;

	TSampleSet() {}

	TSampleSet(const std::vector<state_t>& states)
	: states(states), scores(states.size()) {}

	TSampleSet(const std::vector<state_t>& states, const std::vector<score_t>& scores)
	: states(states), scores(scores) {}

};

template<typename State, typename Score>
void give_size_hint(TSampleSet<State,Score>& list, std::size_t num) {
	list.states.reserve(num);
	list.scores.reserve(num);
}

template<typename State, typename Score>
std::size_t num_samples(const TSampleSet<State,Score>& list) {
	return list.states.size();
}

template<typename State, typename Score>
typename detail::range<typename TSampleSet<State,Score>::state_iterator> states(TSampleSet<State,Score>& list) {
	return std::make_pair(list.states.begin(), list.states.end());
}

template<typename State, typename Score>
typename detail::range<typename TSampleSet<State,Score>::state_const_iterator> states(const TSampleSet<State,Score>& list) {
	return std::make_pair(list.states.begin(), list.states.end());
}

template<typename State, typename Score>
typename detail::range<typename TSampleSet<State,Score>::score_iterator> scores(TSampleSet<State,Score>& list) {
	return std::make_pair(list.scores.begin(), list.scores.end());
}

template<typename State, typename Score>
typename detail::range<typename TSampleSet<State,Score>::score_const_iterator> scores(const TSampleSet<State,Score>& list) {
	return std::make_pair(list.scores.begin(), list.scores.end());
}

/** Addes samples from src to list */
template<typename State, typename Score>
void add_samples(TSampleSet<State,Score>& list, unsigned int num) {
	list.states.resize(list.states.size() + num);
	list.scores.resize(list.scores.size() + num);
}

template<typename State, typename Score>
typename TSampleSet<State,Score>::sample_descriptor add_sample(TSampleSet<State,Score>& list) {
	add_samples(list, 1);
	return list.scores.size() - 1;
}

template<typename State, typename Score>
typename TSampleSet<State,Score>::sample_descriptor add_sample(TSampleSet<State,Score>& list, const State& state, const Score& score) {
	list.states.push_back(state);
	list.scores.push_back(score);
	return list.scores.size() - 1;
}

/** Addes samples from src to list */
template<typename State, typename Score>
void add_samples(TSampleSet<State,Score>& list, const TSampleSet<State,Score>& src) {
	list.scores.insert(list.scores.end(), src.scores.begin(), src.scores.end());
	list.states.insert(list.states.end(), src.states.begin(), src.states.end());
}

template<typename State, typename Score>
TSample<State,Score> get_sample(const TSampleSet<State,Score>& list, typename TSampleSet<State,Score>::sample_descriptor id) {
	return { get_state(list, id), get_score(list, id) };
}

template<typename State, typename Score>
const typename TSampleSet<State,Score>::state_t& get_state(const TSampleSet<State,Score>& list, typename TSampleSet<State,Score>::sample_descriptor id) {
	return list.states[id];
}

template<typename State, typename Score>
const std::vector<typename TSampleSet<State,Score>::state_t>& get_state_list(const TSampleSet<State,Score>& list) {
	return list.states;
}

template<typename State, typename Score>
const typename TSampleSet<State,Score>::score_t& get_score(const TSampleSet<State,Score>& list, typename TSampleSet<State,Score>::sample_descriptor id) {
	return list.scores[id];
}

template<typename State, typename Score>
const std::vector<typename TSampleSet<State,Score>::score_t>& get_score_list(const TSampleSet<State,Score>& list) {
	return list.scores;
}

template<typename State, typename Score>
void set_state(TSampleSet<State,Score>& list, typename TSampleSet<State,Score>::sample_descriptor id, const typename TSampleSet<State,Score>::state_t& state) {
	list.states[id] = state;
}

template<typename State, typename Score>
void set_state_list(TSampleSet<State,Score>& list, const std::vector<typename TSampleSet<State,Score>::state_t>& state_list) {
	assert(state_list.size() == num_samples(list));
	list.states = state_list;
}

template<typename State, typename Score>
void set_score(TSampleSet<State,Score>& list, typename TSampleSet<State,Score>::sample_descriptor id, const typename TSampleSet<State,Score>::score_t& score) {
	list.scores[id] = score;
}

template<typename State, typename Score>
void set_score_list(TSampleSet<State,Score>& list, const std::vector<typename TSampleSet<State,Score>::score_t>& score_list) {
	assert(score_list.size() == num_samples(list));
	list.scores = score_list;
}

}

#endif
