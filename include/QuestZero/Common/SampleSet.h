#ifndef QUESTZERO_SAMPLESET
#define QUESTZERO_SAMPLESET
//---------------------------------------------------------------------------
#include "RandomNumbers.h"
#include "Sample.h"
#include "Sampling.h"
#include "DefaultSampleList.h"
#include <QuestZero/Common/IPrintable.h>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/covariance.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/accumulators/statistics/variates/covariate.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/assert.hpp>
#include <ostream>
#include <algorithm>
#include <vector>
#include <cmath>
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

// basic functionality

/** Gives a hint about how many samples are to be expected. Does not actually add samples! */
template<typename SampleList>
void give_size_hint(SampleList& list, std::size_t num) {}

template<typename SampleList>
std::size_t num_samples(const SampleList& list);

template<typename SampleList>
typename detail::range<typename SampleList::state_iterator> states(SampleList& list);

template<typename SampleList>
typename detail::range<typename SampleList::state_const_iterator> states(const SampleList& list);

template<typename SampleList>
typename detail::range<typename SampleList::score_iterator> scores(SampleList& list);

template<typename SampleList>
typename detail::range<typename SampleList::score_const_iterator> scores(const SampleList& list);

template<typename SampleList>
typename SampleList::sample_descriptor add_sample(SampleList& list);

template<typename SampleList>
typename SampleList::sample_descriptor add_sample(SampleList& list, const typename SampleList::state_t& state, const typename SampleList::score_t& score);

template<typename SampleList>
void add_samples(SampleList& list, unsigned int num);

template<typename SampleList>
void add_samples(SampleList& list, const SampleList& src);

template<typename SampleList>
const TSample<typename SampleList::state_t, typename SampleList::score_t>& get_sample(const SampleList& list, typename SampleList::sample_descriptor id);

template<typename SampleList>
const typename SampleList::state_t& get_state(const SampleList& list, typename SampleList::sample_descriptor id);

template<typename SampleList>
const typename SampleList::score_t& get_score(const SampleList& list, typename SampleList::sample_descriptor id);

template<typename SampleList>
void set_state(SampleList& list, typename SampleList::sample_descriptor, const typename SampleList::state_t& state);

template<typename SampleList>
void set_score(SampleList& list, typename SampleList::sample_descriptor, const typename SampleList::score_t& score_list);

template<typename SampleList>
std::vector<typename SampleList::state_t> get_state_list(const SampleList& list) {
	// default implementation using states iterator range
	std::vector<typename SampleList::state_t> state_list;
	state_list.reserve(num_samples(list));
	for(auto& x : states(list)) {
		state_list.push_back(x);
	}
	return state_list;
}

template<typename SampleList>
std::vector<typename SampleList::score_t> get_score_list(const SampleList& list) {
	// default implementation using scores iterator range
	std::vector<typename SampleList::score_t> score_list;
	score_list.reserve(num_samples(list));
	for(auto& x : scores(list)) {
		score_list.push_back(x);
	}
	return score_list;
}

template<typename SampleList>
void set_state_list(SampleList& list, const std::vector<typename SampleList::state_t>& state_list) {
	// default implementation using states iterator range
	assert(state_list.size() == num_samples(list));
	auto it = state_list.begin();
	for(auto& x : states(list)) {
		x = *it;
		++it;
	}
}

template<typename SampleList>
void set_score_list(SampleList& list, const std::vector<typename SampleList::score_t>& score_list) {
	// default implementation using scores iterator range
	assert(score_list.size() == num_samples(list));
	auto it = score_list.begin();
	for(auto& x : scores(list)) {
		x = *it;
		++it;
	}
}

// now the sampling functionality

template<typename SampleList, typename Function>
std::vector<typename SampleList::score_t> evaluate(const SampleList& list, Function f) {
	std::vector<typename SampleList::score_t> score_list;
	score_list.reserve(num_samples(list));
	for(auto& x : states(list)) {
		score_list.push_back(f(x));
	}
	return score_list;
}

template<typename SampleList, typename Function>
std::vector<typename SampleList::score_t> evaluate_parallel(const SampleList& list, Function f) {
	return f(get_state_list(list));
}

/** Selects random samples from the sample list using a probability proportional to the score */
template<typename SampleList>
SampleList draw_by_score(const SampleList& list, size_t num_samples) {
	std::vector<size_t> picked = SamplingTools::DrawFromDistribution(get_score_list(list), num_samples);
	SampleList picked_samples;
	give_size_hint(picked_samples, picked.size());
	for(size_t p : picked) {
		auto id = add_sample(picked_samples);
		set_state(picked_samples, id, get_state(list, p));
		set_score(picked_samples, id, get_score(list, p));
	}
	return picked_samples;
}

/** Multiplies the score of each particle by a value obtained from an importance function */
template<typename SampleList, typename ImportanceFunction>
void importance_scoring(SampleList& list, ImportanceFunction importance_function) {
	std::vector<typename SampleList::score_t> importance = evaluate(list, importance_function);
	auto it = importance.begin();
	for(auto& x : scores(list)) {
		x *= *it;
		++it;
	}
}

template<typename SampleList, typename Function>
void compute_likelihood(SampleList& list, const Function& f) {
	std::vector<typename SampleList::score_t> importance = evaluate(list, f);
	auto it = importance.begin();
	for(auto& x : scores(list)) {
		x = *it;
		++it;
	}
}

/** Transforms all states by a function */
template<typename SampleList, typename Function>
void transform_states(SampleList& list, const Function& f) {
	for(auto& x : states(list)) {
		x = f(x);
	}
}

/** Transforms all scores with a function */
template<typename SampleList, typename Function>
void transform_scores(SampleList& list, const Function& f) {
	for(auto& x : scores(list)) {
		x = f(x);
	}
}

/** Adds noise to all states (invalidates scores) */
template<typename SampleList, typename Space, typename K>
void randomize_states(SampleList& list, const Space& space, const std::vector<K>& noise) {
	transform_states(list, [&space, &noise](const typename SampleList::state_t& state) {
		return space.random(state, noise);
	});
}

/** Returns the best element defined by being smallest under the given comparer applied on the sample score */
template<typename SampleList, typename ScoreComparer>
typename SampleList::sample_descriptor find_best_by_score(const SampleList& list, ScoreComparer c) {
	auto range = scores(list);
	auto it = std::min_element(range.begin(), range.end(), c);
	return it - range.begin(); // FIXME only works for random access containers!
}

template<typename SampleList, typename ScoreComparer>
typename SampleList::sample_descriptor find_worst_by_score(const SampleList& list, ScoreComparer c) {
	auto range = scores(list);
	auto it = std::max_element(range.begin(), range.end(), c);
	return it - range.begin(); // FIXME only works for random access containers!
}

template<typename SampleList, typename ScoreComparer>
SampleList pick_best(const SampleList& list, unsigned int num, ScoreComparer c) {
	// TODO Perhaps this is faster with partial_sort_copy? Would require a samples(list) function!
	struct Item {
		typename SampleList::sample_descriptor id;
		typename SampleList::score_t score;
	};
	std::vector<Item> items;
	items.reserve(num_samples(list));
	auto scores_range = scores(list);
	for(auto it=scores_range.begin(); it!=scores_range.end(); ++it) {
		typename SampleList::sample_descriptor id = it - scores_range.begin(); // FIXME only works for random access!
		items.push_back({id, *it});
	}
	std::partial_sort(items.begin(), items.begin() + num, items.end(), [&c](const Item& x, const Item& y) {
		return c(x.score, y.score);
	});
	SampleList best_list;
	give_size_hint(list, num);
	std::for_each(items.begin(), items.begin() + num, [&best_list, &list](const Item& i) {
		auto id = add_sample(best_list);
		set_state(best_list, id, get_state(list, i.id));
		set_score(best_list, id, i.score);
	});
	return best_list;
}

/** Prints all samples as a list */
template<typename SampleList>
void print(const SampleList& sample_list, std::ostream& os, bool in_lines=false) {
	os << "[Samples = {";
	auto state_range = states(sample_list);
	auto score_range = scores(sample_list);
	auto state_it = state_range.begin();
	auto score_it = score_range.begin();
	while(state_it != state_range.end()) {
		os << "[state=" << *state_it << ", score=" << *score_it << "]";
		if(in_lines) {
			os << std::endl;
		}
		else {
			os << ", ";
		}
		++state_it;
		++score_it;
	}
	os << "}]" << std::endl;
}

template<typename SampleList>
void print_scores(const SampleList& sample_list, std::ostream& os) {
	os << "[Samples Scores = {";
	for(auto& x : scores(sample_list)) {
		os << x << ", ";
	}
	os << "}]" << std::endl;
}

template<typename SampleList>
void print_score_info(const SampleList& sample_list, std::ostream& os) {
	using namespace boost::accumulators;
	accumulator_set<double, stats<tag::min, tag::max, tag::mean, tag::variance>> score_acc;
	auto range = scores(sample_list);
	score_acc = std::for_each(range.begin(), range.end(), score_acc);
	os << "Sample Scores: Min=" << min(score_acc)
			<< " / Mean=" << mean(score_acc)
			<< " / Max=" << max(score_acc)
			<< " / Dev=" << std::sqrt(variance(score_acc)) << std::endl;
}

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
