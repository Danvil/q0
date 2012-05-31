#ifndef QUESTZERO_SAMPLESET
#define QUESTZERO_SAMPLESET
//---------------------------------------------------------------------------
#include "Sample.h"
#include "RandomNumbers.h"
#include <QuestZero/Common/IPrintable.h>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/covariance.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/accumulators/statistics/variates/covariate.hpp>
#include <ostream>
#include <algorithm>
#include <vector>
#include <cmath>
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

struct EmptySampleSetException {};

struct CanNotNormalizeZeroListException {};

struct InvalidDistributionException {};

namespace SamplingTools
{
	/** Normalizes a list of elements */
	template<typename K>
	double Sum(const std::vector<K>& v) {
		double sum = 0;
		for(std::vector<double>::const_iterator it=v.begin(); it!=v.end(); ++it) {
			sum += double(*it);
		}
		return sum;
	}

	/** Normalizes a list of elements */
	template<typename K>
	std::vector<double> Normalize(const std::vector<K>& v, double sum) {
		if(sum == 0) {
			throw CanNotNormalizeZeroListException();
		}
		double scl = 1.0 / sum;
		std::vector<double> weights;
		weights.resize(v.size());
		for(size_t i=0; i<v.size(); i++) {
			weights[i] = double(v[i]) * scl;
		}
		return weights;
	}

	/** Normalizes a list of elements */
	template<typename K>
	std::vector<double> Normalize(const std::vector<K>& v) {
		double sum = Sum(v);
		return Normalize(v, sum);
	}

	/** Computes the non-unit density function of a discrete probability distribution
	 * Non-unit means that the distribution is not normalized to be 1
	 */
	template<typename K>
	static std::vector<double> ComputeDensityUnnormalized(const std::vector<K>& distribution) {
		if(distribution.size() == 0) {
			throw CanNotNormalizeZeroListException();
		}
		// we first build the density using the unnormalized weights
		std::vector<double> density;
		density.reserve(distribution.size());
		double last = 0;
		for(size_t i=0; i<distribution.size(); i++) {
			last += double(distribution[i]);
			density.push_back(last);
		}
		// 'last' is now the total sum of all scores
		return density;
	}

	/** Computes the density function of a discrete probability distribution */
	template<typename K>
	std::vector<double> ComputeDensity(const std::vector<K>& scores) {
		// we first build the density using the unnormalized scores
		std::vector<double> density = ComputeDensityUnnormalized(scores);
		// normalize density
		return Normalize(density, density.back());
	}

	/** Finds the index of the first value larger than the given value */
	inline
	size_t FindIndexOfFirstLargerThan(const std::vector<double>& list, double val) {
		// TODO use bisection search here if list is sorted!
		for(std::vector<double>::const_iterator it=list.begin(); it!=list.end(); ++it) {
			if(*it >= val) {
				return it - list.begin();
			}
		}
		throw InvalidDistributionException(); // TODO throw a different exception here!
	}

	/** Randomly picks one elements from a density function of a discrete probability distribution */
	inline
	size_t SampleDensity(const std::vector<double>& density) {
		double r = RandomNumbers::Uniform<double>(density.back());
		return FindIndexOfFirstLargerThan(density, r);
		// TODO catch possible exceptions and transfer to InvalidDistributionException
	}

	/** Randomly picks several elements from a density function of a discrete probability distribution */
	inline
	std::vector<size_t> SampleDensity(const std::vector<double>& density, size_t count) {
		std::vector<size_t> picked;
		picked.reserve(count);
		for(size_t i=0; i<count; ++i) {
			size_t p = SampleDensity(density);
			picked.push_back(p);
		}
		return picked;
	}

	/** Randomly picks several elements from a discrete probability distribution */
	template<typename K>
	std::vector<size_t> DrawFromDistribution(const std::vector<K>& distribution, size_t count) {
		std::vector<double> density = ComputeDensityUnnormalized(distribution);
		return SampleDensity(density, count);
	}

}

/** A set of states with scores providing several convenience functions */
template<typename State, typename Score>
struct TSampleSet
: public IPrintable
{
	typedef TSample<State,Score> Sample;

	TSampleSet() { }

	TSampleSet(const std::vector<State>& states) {
		Add(states);
		ResetWeights();
	}

	TSampleSet(const std::vector<Sample>& samples) {
		Add(samples);
	}

	~TSampleSet() {}

	/** Checks if the set is empty */
	bool IsEmpty() const {
		return Size() == 0;
	}

	/** Number of samples */
	size_t Size() const {
		return states_.size();
	}

	/** Reserves room for n samples (does not change size!) */
	void Reserve(size_t n) {
		states_.reserve(n);
		scores_.reserve(n);
		weights_.reserve(n);
	}

	/** Resizes samples storage (will contain garbage!) */
	void Resize(size_t n) {
		states_.resize(n);
		scores_.resize(n);
		weights_.resize(n);
	}

	/** Adds a sample */
	void Add(const State& state, Score score=C_UNKNOWN_SCORE) {
		states_.push_back(state);
		scores_.push_back(score);
		weights_.push_back(0.0);
	}

	/** Adds a sample */
	void Add(const Sample& sample) {
		Score score = sample.isScoreKnown() ? sample.score() : C_UNKNOWN_SCORE;
		Add(sample.state(), score);
	}

	/** Adds states (and sets score to unkown) */
	void Add(const std::vector<State>& states) {
		states_.insert(states_.end(), states.begin(), states.end());
		scores_.insert(scores_.end(), states.size(), C_UNKNOWN_SCORE);
		weights_.insert(weights_.end(), states.size(), 0.0);
	}

	/** Adds samples from another sample set */
	void Add(const TSampleSet& data) {
		states_.insert(states_.end(), data.states_.begin(), data.states_.end());
		scores_.insert(scores_.end(), data.scores_.begin(), data.scores_.end());
		weights_.insert(weights_.end(), data.weights_.begin(), data.weights_.end());
	}

	/** Adds samples from another sample set */
	void Add(const std::vector<Sample>& samples) {
		Reserve(Size() + samples.size());
		for(size_t i=0; i<samples.size(); i++) {
			Add(samples[i].state(), samples[i].score());
		}
	}

	/** Gets i-th state vector */
	const State& state(size_t i) const {
		return states_[i];
	}

	/** Gets i-th score */
	Score score(size_t i) const {
		return scores_[i];
	}

	/** Gets i-th score */
	double weight(size_t i) const {
		return weights_[i];
	}

	/** Sets i-th state vector */
	void set_state(size_t i, const State& u) {
		states_[i] = u;
	}

	/** Sets i-th score */
	void set_score(size_t i, Score s) {
		scores_[i] = s;
	}

	/** Sets i-th score */
	void set_weight(size_t i, double s) {
		weights_[i] = s;
	}

	/** Creates a sample for entry i */
	Sample CreateSample(size_t i) const {
		return Sample(state(i), score(i), weight(i));
	}

	/** Gets list of all states in the set */
	const std::vector<State>& states() const { return states_; }

	/** Gets list of all scores in the set */
	const std::vector<Score>& scores() const { return scores_; }

	const std::vector<double>& weights() const { return weights_; }

	/** Sets all scores */
	void SetAllScores(const std::vector<Score>& scores) {
		assert(scores.size() == Size());
		scores_ = scores;
	}

	/** Sets all scores */
	void SetAllScores(Score score) {
		for(size_t i=0; i<scores_.size(); i++) {
			scores_[i] = score;
		}
	}

	void SetAllWeights(double weight) {
		for(size_t i=0; i<weights_.size(); i++) {
			weights_[i] = weight;
		}
	}

	void ResetWeights() {
		double w = 1.0 / double(weights_.size());
		SetAllWeights(w);
	}

	std::vector<double> ComputeScoreDensity() {
		return SamplingTools::ComputeDensity(scores_);
	}

	std::vector<double> ComputeWeightDensity() {
		return SamplingTools::ComputeDensity(weights_);
	}

	TSampleSet DrawByScore(unsigned int n) const {
		std::vector<size_t> picked = SamplingTools::DrawFromDistribution(scores_, n);
		TSampleSet picked_samples;
		picked_samples.Reserve(picked.size());
		for(unsigned int i=0; i<picked.size(); ++i) {
			size_t p = picked[i];
			picked_samples.Add(state(p), score(p));
		}
		return picked_samples;
	}

//	/** Randomly picks n samples using a probability which is proportional to the sample score */
//	TSampleSet DrawByScore(unsigned int n) const {
//		return DrawByScore(scores_, n);
//	}

	/** Randomly picks n samples using a probability which is proportional to the sample weight and sets weight to 1/n */
	void Resample(size_t n) {
		std::vector<size_t> picked = SamplingTools::DrawFromDistribution(weights_, n);
		std::vector<State> old_states = states_;
		std::vector<Score> old_scores = scores_;
		Resize(picked.size());
		double w = 1.0 / picked.size();
		for(size_t i=0; i<picked.size(); ++i) {
			size_t p = picked[i];
			BOOST_ASSERT(p < old_states.size());
			states_[i] = old_states[p];
			scores_[i] = old_scores[p];
			weights_[i] = w;
		}
	}

	template<class Op>
	void WeightedResample(const Op& importance_function) {
		// evaluate importance function
		std::vector<Score> importance = importance_function(states());
		//std::vector<double> importance_weights = SamplingTools::Normalize(importance);
		// weights are normalized afterwards, so we do not have to normalize importance weights
		// pick random samples
		std::vector<double> density = SamplingTools::ComputeDensityUnnormalized(importance);
		std::vector<size_t> picked = SamplingTools::SampleDensity(density, Size());
		// pick samples
		std::vector<State> old_states = states_;
		std::vector<Score> old_scores = scores_;
		std::vector<double> old_weights = weights_;
		states_.resize(picked.size());
		scores_.resize(picked.size());
		weights_.resize(picked.size());
		for(unsigned int i=0; i<picked.size(); i++) {
			size_t p = picked[i];
			states_[i] = old_states[p];
			scores_[i] = importance[p]; // set to importance // TODO leave score unchanged?
			weights_[i] = old_weights[p] / double(importance[p]); // adapt sample weight!
		}
		// normalize weights
		SamplingTools::Normalize(weights_);
	}

	template<class Op>
	void ComputeLikelihood(const Op& f) {
		scores_ = f(states());
		for(unsigned int i=0; i<Size(); i++) {
			weights_[i] *= double(scores_[i]);
		}
		// normalize weights
		SamplingTools::Normalize(weights_);
	}

private:
	template<class Space, typename K>
	struct FunctionRandomizeState
	{
		FunctionRandomizeState(const Space& space, const std::vector<K>& noise)
		: space_(space), noise_(noise) {}
		State operator()(const State& u) {
			return space_.random(u, noise_);
		}
	private:
		const Space& space_;
		const std::vector<K>& noise_;
	};

public:
	/** Adds noise to all states (invalidates scores) */
	template<class Space, typename K>
	void RandomizeStates(const Space& space, const std::vector<K>& noise) {
		TransformStates(FunctionRandomizeState<Space,K>(space, noise));
	}

	/** Transforms all states by a function (invalidates scores) */
	template<class Op>
	void TransformStates(const Op& f) {
		std::transform(states_.begin(), states_.end(), states_.begin(), f);
	}

	/** Transforms all scores with a function */
	template<class Op>
	void TransformScores(const Op& f) {
		std::transform(scores_.begin(), scores_.end(), scores_.begin(), f);
	}

//	/** Evaluates only unknown samples */
//	template<class Function>
//	void EvaluateUnknown(const Function& f) {
//		if(this->AreAllStatesUnevaluated()) {
//			std::vector<State> unknown_states = this->states();
//			std::vector<Score> scores = f(unknown_states);
//			this->setScores(scores);
//		} else {
//			std::vector<size_t> unknown_ids;
//			std::vector<State> unknown_states = this->GetStatesWithUnknownScore(unknown_ids);
//			std::vector<Score> scores = f(unknown_states);
//			this->setScores(scores, unknown_ids);
//		}
//	}

//	/** Computes scores for all states using an evaluation functin */
//	template<class Function>
//	void EvaluateAll(const Function& f) {
//		scores_ = f(states());
//	}

	template<class CMP>
	void FindBestAndWorstScore(Score& best, Score& worst) const {
		if(Size() == 0) {
			throw EmptySampleSetException();
		}
		best = scores_[0];
		worst = best;
		CMP c;
		for(typename std::vector<Score>::const_iterator it=scores_.begin(); it!=scores_.end(); ++it) {
			Score current = *it;
			if(c(current, best)) {
				best = current;
			}
			if(c(worst, current)) {
				worst = current;
			}
		}
	}

	template<class CMP>
	size_t SearchWorst() const {
		if(Size() == 0) {
			throw EmptySampleSetException();
		}
		size_t worst_id = 0;
		Score worst = scores_[0];
		CMP c;
		for(typename std::vector<Score>::const_iterator it=scores_.begin(); it!=scores_.end(); ++it) {
			if(c(worst, *it)) {
				worst = *it;
				worst_id = it - scores_.begin();
			}
		}
	}

	template<class CMP>
	void SearchBestAndWorst(size_t& best_id, size_t& worst_id) const {
		if(Size() == 0) {
			throw EmptySampleSetException();
		}
		Score best = scores_[0];
		best_id = 0;
		Score worst = best;
		worst_id = 0;
		CMP c;
		typename std::vector<Score>::const_iterator it;
		size_t i;
		for(it=scores_.begin(), i=0; it!=scores_.end(); ++it, i++) {
			Score current = *it;
			if(c(current, best)) {
				best_id = i;
				best = current;
			}
			if(c(worst, current)) {
				worst_id = i;
				worst = current;
			}
		}
	}

	/** Finds the best score */
	template<class CMP>
	Score FindBestScore() const {
		if(Size() == 0) {
			throw EmptySampleSetException();
		}
		typename std::vector<Score>::const_iterator it_best = std::min_element(scores_.begin(), scores_.end(), CMP());
		return *it_best;
	}

	/** Finds the best sample */
	template<class CMP>
	Sample FindBestSample() const {
		if(Size() == 0) {
			throw EmptySampleSetException();
		}
		typename std::vector<Score>::const_iterator it_best = std::min_element(scores_.begin(), scores_.end(), CMP());
		return CreateSample(it_best - scores_.begin());
//		size_t best_index = 0;
//		Score best_score = score(best_index);
//		for(size_t i=0; i<Size(); i++) {
//			Score current_score = score(i);
//			// FIXME check if score is known!
//			if(cmp(current_score, best_score)) {
//				best_score = current_score;
//				best_index = i;
//			}
//		}
////		if(it_best->isScoreUnknown()) {
////			throw std::runtime_error("Can not get best sample if all samples are unevaluated!");
////		}
//		return this->CreateSample(best_index);
	}

private:
	struct IndexAndScore {
		IndexAndScore() {}
		IndexAndScore(size_t index, Score score) : index_(index), score_(score) {}
		size_t index_;
		Score score_;
	};

	template<class CMP>
	struct CompareIndexAndScore {
		bool operator()(const IndexAndScore& a, const IndexAndScore& b) {
			return CMP::compare(a.score_, b.score_);
		}
	};

public:
//	/** Creates and list with the n samples with best score */
//	template<class CMP>
//	std::vector<Sample> FindBestSamples(size_t n) const {
//		// FIXME use better algorithm: http://en.wikipedia.org/wiki/Selection_algorithm
//		std::vector<Sample> samples;
//		samples.reserve(Size());
//		for(size_t i=0; i<Size(); i++) {
//			samples.push_back(Sample(state(i), score(i)));
//		}
//		// use partial sort to find the n best samples
//		std::partial_sort(samples.begin(), samples.begin() + n, samples.end(), CMP());
//		// FIXME sort copies around states! Better build an array with indices and copy later.
//		return std::vector<Sample>(samples.begin(), samples.begin() + n);
//	}

	/** Creates and returns a sample set with the n samples with best score */
	template<class CMP>
	TSampleSet FindBestSamples(size_t n) const {
		// build vector of indices and scores
		std::vector<IndexAndScore> v;
		v.reserve(Size());
		for(size_t i=0; i<Size(); i++) {
			v.push_back(IndexAndScore(i, score(i)));
		}
		// use partial sort to find the n best samples
		std::partial_sort(v.begin(), v.begin() + n, v.end(), CompareIndexAndScore<CMP>());
		// create Sample set
		TSampleSet samples;
		samples.Reserve(n);
		for(typename std::vector<IndexAndScore>::const_iterator it=v.begin(); it!=v.begin() + n; ++it) {
			samples.Add(state(it->index_), it->score_);
		}
		return samples;
	}

	/** Prints all samples as a list */
	void print(std::ostream& os) const {
		os << "[Samples = {";
		for(size_t i=0; i<Size(); i++) {
			os << CreateSample(i) << ", ";
		}
		os << "}]" << std::endl;
	}

	void printScores(std::ostream& os) const {
		os << "[Sample Scores = {";
		for(size_t i=0; i<Size(); i++) {
			os << score(i) << ", ";
		}
		os << "}]" << std::endl;
	}

	void printScoresAndWeights(std::ostream& os) const {
		os << "[Sample Weights/Scores = {";
		for(size_t i=0; i<Size(); i++) {
			os << "(" << weight(i) << ", " << score(i) << "), ";
		}
		os << "}]" << std::endl;
	}

	void PrintScoreInfo(std::ostream& os) const {
		using namespace boost::accumulators;
		accumulator_set<double, stats<tag::min, tag::max, tag::mean, tag::variance>> score_acc;
		score_acc = std::for_each(scores_.begin(), scores_.end(), score_acc);
		Score min = min(score_acc);
		Score max = max(score_acc);
		Score mean = mean(score_acc);
		Score dev = std::sqrt(variance(score_acc));
		os << "Sample Scores: Min=" << min << " / Mean=" << mean << " / Max=" << max << " / Dev=" << dev << std::endl;
	}

	/** Prints all samples, one line per sample */
	void printInLines(std::ostream& os) const {
		os << "[Samples = {\n";
		for(size_t i=0; i<Size(); i++) {
			os << "\t" << CreateSample(i) << "\n";
		}
		os << "}]" << std::endl;
	}

private:
	// list of particle states
	std::vector<State> states_;

	// list of particle scores
	std::vector<Score> scores_;

	// list of particle weights in the representation of the probability distribution
	std::vector<double> weights_;

};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
