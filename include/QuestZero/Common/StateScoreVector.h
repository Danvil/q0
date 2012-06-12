/*
 * StateScoreVector.h
 *
 *  Created on: May 31, 2012
 *      Author: david
 */

#ifndef Q0_STATESCOREVECTOR_H_
#define Q0_STATESCOREVECTOR_H_

#include "Sample.h"
#include "SampleSet.h"
#include <boost/assert.hpp>
#include <vector>

namespace Q0 {

// a default data structure for a state list

template<typename State, typename Score>
struct StateScoreVector
{
	std::vector<State> states;
	std::vector<Score> scores;

	StateScoreVector() {}

	StateScoreVector(const std::vector<State>& states)
	: states(states), scores(states.size()) {}

	StateScoreVector(const std::vector<State>& states, const std::vector<Score>& scores)
	: states(states), scores(scores) {}

};

template<typename State, typename Score>
struct SampleSetTraits<StateScoreVector<State,Score> >
{
	typedef State state_t;
	typedef Score score_t;
	typedef std::size_t sample_descriptor;
//	typedef typename StateScoreVector<State,Score>::sample_iterator sample_iterator;
//	typedef typename StateScoreVector<State,Score>::sample_const_iterator sample_const_iterator;
};

template<typename State, typename Score>
void give_size_hint(StateScoreVector<State,Score>& list, std::size_t num) {
	list.states.reserve(num);
	list.scores.reserve(num);
}

template<typename State, typename Score>
std::size_t num_samples(const StateScoreVector<State,Score>& list) {
	return list.states.size();
}

/** Addes samples from src to list */
template<typename State, typename Score>
void add_samples(StateScoreVector<State,Score>& list, unsigned int num) {
	list.states.resize(list.states.size() + num);
	list.scores.resize(list.scores.size() + num);
}

template<typename State, typename Score>
typename SampleSetTraits<StateScoreVector<State,Score>>::sample_descriptor add_sample(StateScoreVector<State,Score>& list) {
	add_samples(list, 1);
	return list.scores.size() - 1;
}

//template<typename State, typename Score>
//typename SampleSetTraits<StateScoreVector<State,Score>>::sample_descriptor add_sample(StateScoreVector<State,Score>& list, const State& state, const Score& score) {
//	list.states.push_back(state);
//	list.scores.push_back(score);
//	return list.scores.size() - 1;
//}

/** Addes samples from src to list */
template<typename State, typename Score>
void add_samples(StateScoreVector<State,Score>& list, const StateScoreVector<State,Score>& src) {
	list.states.insert(list.states.end(), src.states.begin(), src.states.end());
	list.scores.insert(list.scores.end(), src.scores.begin(), src.scores.end());
}

template<typename State, typename Score>
const typename SampleSetTraits<StateScoreVector<State,Score>>::state_t& get_state(const StateScoreVector<State,Score>& list, typename SampleSetTraits<StateScoreVector<State,Score>>::sample_descriptor id) {
	return list.states[id];
}

template<typename State, typename Score>
const std::vector<typename SampleSetTraits<StateScoreVector<State,Score>>::state_t>& get_state_list(const StateScoreVector<State,Score>& list) {
	return list.states;
}

template<typename State, typename Score>
const typename SampleSetTraits<StateScoreVector<State,Score>>::score_t& get_score(const StateScoreVector<State,Score>& list, typename SampleSetTraits<StateScoreVector<State,Score>>::sample_descriptor id) {
	return list.scores[id];
}

template<typename State, typename Score>
const std::vector<typename SampleSetTraits<StateScoreVector<State,Score>>::score_t>& get_score_list(const StateScoreVector<State,Score>& list) {
	return list.scores;
}

template<typename State, typename Score>
void set_state(StateScoreVector<State,Score>& list, typename SampleSetTraits<StateScoreVector<State,Score>>::sample_descriptor id, const typename SampleSetTraits<StateScoreVector<State,Score>>::state_t& state) {
	list.states[id] = state;
}

template<typename State, typename Score>
void set_state_list(StateScoreVector<State,Score>& list, const std::vector<typename SampleSetTraits<StateScoreVector<State,Score>>::state_t>& state_list) {
	assert(state_list.size() == num_samples(list));
	list.states = state_list;
}

template<typename State, typename Score>
void set_score(StateScoreVector<State,Score>& list, typename SampleSetTraits<StateScoreVector<State,Score>>::sample_descriptor id, const typename SampleSetTraits<StateScoreVector<State,Score>>::score_t& score) {
	list.scores[id] = score;
}

template<typename State, typename Score>
void set_score_list(StateScoreVector<State,Score>& list, const std::vector<typename SampleSetTraits<StateScoreVector<State,Score>>::score_t>& score_list) {
	assert(score_list.size() == num_samples(list));
	list.scores = score_list;
}

}

#endif
