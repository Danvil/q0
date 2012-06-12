/*
 * SampleVector.h
 *
 *  Created on: Jun 07, 2012
 *      Author: david
 */

#ifndef Q0_SAMPLEVECTOR_H_
#define Q0_SAMPLEVECTOR_H_

#include "Sample.h"
#include "SampleSet.h"
#include <boost/assert.hpp>
#include <vector>

namespace Q0 {

/**
 * This file provides the necessary interface functions to treat
 * a std::vector<Sample> as a SampleSet.
 */

/* Sample traits give state and score type of a Sample type */
template<typename Sample>
struct SampleTraits {
	typedef typename Sample::state_t state_t;
	typedef typename Sample::score_t score_t;
};

/** Gets the state of a sample */
template<typename Sample>
const typename SampleTraits<Sample>::state_t& get_state(const Sample& sample);

/** Gets the score of a sample */
template<typename Sample>
const typename SampleTraits<Sample>::score_t& get_score(const Sample& sample);

/** Sets the state of a sample */
template<typename Sample>
void set_state(const Sample& sample, const typename SampleTraits<Sample>::state_t& state);

/** Sets the score of a sample */
template<typename Sample>
void set_score(const Sample& sample, const typename SampleTraits<Sample>::score_t& score);

/** SampleSet traits overload */
template<typename Sample>
struct SampleSetTraits<std::vector<Sample>>
{
	typedef typename SampleTraits<Sample>::state_t state_t;
	typedef typename SampleTraits<Sample>::score_t score_t;
	typedef std::size_t sample_descriptor;
};

template<typename Sample>
void give_size_hint(std::vector<Sample>& list, std::size_t num) {
	list.reserve(num);
}

template<typename Sample>
std::size_t num_samples(const std::vector<Sample>& list) {
	return list.size();
}

template<typename Sample>
void add_samples(std::vector<Sample>& list, unsigned int num) {
	list.resize(list.size() + num);
}

template<typename Sample>
typename SampleSetTraits<std::vector<Sample>>::sample_descriptor add_sample(std::vector<Sample>& list) {
	add_samples(list, 1);
	return list.size() - 1;
}

template<typename Sample>
void add_samples(std::vector<Sample>& list, const std::vector<Sample>& src) {
	list.insert(list.end(), src.begin(), src.end());
}

template<typename Sample>
const typename SampleSetTraits<std::vector<Sample>>::state_t& get_state(const std::vector<Sample>& list, typename SampleSetTraits<std::vector<Sample>>::sample_descriptor id) {
	return get_state(list[id]);
}

template<typename Sample>
const typename SampleSetTraits<std::vector<Sample>>::score_t& get_score(const std::vector<Sample>& list, typename SampleSetTraits<std::vector<Sample>>::sample_descriptor id) {
	return get_score(list[id]);
}

template<typename Sample>
void set_state(std::vector<Sample>& list, typename std::vector<Sample>::size_type id, const typename SampleSetTraits<std::vector<Sample>>::state_t& state) {
	set_state(list[id], state);
}

template<typename Sample>
void set_score(std::vector<Sample>& list, typename std::vector<Sample>::size_type id, const typename SampleSetTraits<std::vector<Sample>>::score_t& score) {
	set_score(list[id], score);
}

//template<typename Sample>
//void set_sample(std::vector<Sample>& list, typename std::vector<Sample>::size_type id, const Sample& sample) {
//	list[id] = sample;
//}
//
//template<typename Sample>
//typename std::vector<Sample>::size_type add_sample(std::vector<Sample>& list, const Sample& sample) {
//	list.push_back(sample);
//	return list.scores.size() - 1;
//}
//
//template<typename Sample>
//const Sample& get_sample(const std::vector<Sample>& list, typename std::vector<Sample>::size_type id) {
//	return list[id];
//}

template<typename Sample>
std::vector<typename SampleSetTraits<std::vector<Sample>>::state_t> get_state_list(const std::vector<Sample>& list) {
	std::vector<typename Sample::state_t> states;
	states.reserve(num_samples(list));
	for(auto id : samples(list)) {
		states.push_back(get_state(list, id));
	}
	return states;
}

template<typename Sample>
std::vector<typename SampleSetTraits<std::vector<Sample>>::score_t> get_score_list(const std::vector<Sample>& list) {
	std::vector<typename Sample::score_t> scores;
	scores.reserve(num_samples(list));
	for(auto id : samples(list)) {
		scores.push_back(get_score(list, id));
	}
	return scores;
}

template<typename Sample>
void set_state_list(std::vector<Sample>& list, const std::vector<typename SampleSetTraits<std::vector<Sample>>::state_t>& state_list) {
	assert(state_list.size() == num_samples(list));
	auto it = state_list.begin();
	for(auto id : samples(list)) {
		set_state(list, id, *it);
		++it;
	}
}

template<typename Sample>
void set_score_list(std::vector<Sample>& list, const std::vector<typename SampleSetTraits<std::vector<Sample>>::score_t>& score_list) {
	assert(score_list.size() == num_samples(list));
	auto it = score_list.begin();
	for(auto id : samples(list)) {
		set_score(list, id, *it);
		++it;
	}
}

}

#endif
