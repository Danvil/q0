#ifndef QUESTZERO_SAMPLESET
#define QUESTZERO_SAMPLESET
//---------------------------------------------------------------------------
#include "Sample.h"
#include "RandomNumbers.h"
#include <Danvil/Print.h>
#include <Danvil/Ptr.h>
#include <boost/foreach.hpp>
#include <vector>
#include <algorithm>
#include <ostream>
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

/** A set of states with scores providing several convenience functions */
template<typename State, typename Score>
struct TSampleSet
: public Danvil::Print::IPrintable
{
	struct CanNotNormalizeZeroListException {};

	struct InvalidDistributionException {};

	typedef TSample<State,Score> Sample;

	TSampleSet() { }

	TSampleSet(const std::vector<State>& states) {
		Add(states);
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
	}

	/** Adds a sample */
	void Add(const State& state, Score score=C_UNKNOWN_SCORE) {
		states_.push_back(state);
		scores_.push_back(score);
	}

	/** Adds a sample */
	void Add(const Sample& sample) {
		Score score = sample.isScoreKnown() ? sample.score() : C_UNKNOWN_SCORE;
		Add(sample.state(), score);
	}

	/** Adds states (and sets score to unkown) */
	void Add(const std::vector<State>& states) {
		states_.insert(states_.end(), states.begin(), states.end());
		scores_.insert(scores_.end(), C_UNKNOWN_SCORE);
	}

	/** Adds samples from another sample set */
	void Add(const TSampleSet& data) {
		states_.insert(states_.end(), data.states_.begin(), data.states_.end());
		scores_.insert(scores_.end(), data.scores_.begin(), data.scores_.end());
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

	/** Sets i-th state vector */
	void set_state(size_t i, const State& u) {
		states_[i] = u;
	}

	/** Sets i-th score */
	void set_score(size_t i, Score s) {
		scores_[i] = s;
	}

	/** Creates a sample for entry i */
	Sample CreateSample(size_t i) const {
		return Sample(state(i), score(i));
	}

	/** Gets list of all states in the set */
	const std::vector<State>& states() const { return states_; }

	/** Gets list of all scores in the set */
	const std::vector<Score>& scores() const { return scores_; }

	/** Sets all scores */
	void SetAllScores(const std::vector<Score>& scores) {
		assert(scores.size() == Size());
		scores_ = scores;
	}

//	void SetScoresIndexed(const std::vector<Score>& scores, const std::vector<size_t>& indices) {
//		for(size_t i=0; i<count(); i++) {
//			setScore(indices[i], scores[i]);
//		}
//	}

//	std::vector<State> GetStatesWithUnknownScore(std::vector<size_t>& indices) const {
//		indices.clear();
//		std::vector<State> unknown_states;
//		indices.reserve(count());
//		unknown_states.reserve(count());
//		size_t i = 0;
//		for(typename std::vector<Sample>::const_iterator it=_samples.begin(); it!=_samples.end(); ++it, i++) {
//			if(it->isScoreUnknown()) {
//				indices.push_back(i);
//				unknown_states.push_back(it->state());
//			}
//		}
//		return unknown_states;
//	}

//	bool AreAllStatesUnevaluated() const {
//		for(typename std::vector<Sample>::const_iterator it=_samples.begin(); it!=_samples.end(); ++it) {
//			if(it->isScoreKnown()) {
//				return false;
//			}
//		}
//		return true;
//	}

private:
	/** Computes the density of the probability distribution got by normalizing the samples scores */
	std::vector<double> ComputeScoreDensity() const {
		if(Size() == 0) {
			throw CanNotNormalizeZeroListException();
		}
		// we first build the density using the unnormalized scores
		std::vector<double> density;
		density.reserve(Size());
		double last = 0;
		for(size_t i=0; i<Size(); i++) {
			last += (double)score(i);
			density.push_back(last);
		}
		// last is now the total sum of all scores
		// using this we perform the normalization
		if(last == 0) {
			throw CanNotNormalizeZeroListException();
		}
		double scl = 1.0 / last;
		BOOST_FOREACH(double& x, density) {
			x *= scl;
		}
		return density;
	}

	/** Finds the index of the first value larger than the given value */
	static size_t FindIndexOfFirstLargerThan(const std::vector<double>& list, double val) {
		// FIXME use bisection search here if list is sorted!
		size_t index = 0;
		BOOST_FOREACH(double x, list) {
			if(x >= val) {
				return index;
			}
			index++;
		}
		throw InvalidDistributionException(); // TODO throw a different exception here!
	}

	/** Randomly picks a value from a discreet probability distribution */
	static size_t RandomPickFromDensity(const std::vector<double>& density) {
		double r = RandomNumbers::Uniform<double>();
		return FindIndexOfFirstLargerThan(density, r);
		// TODO catch possible exceptions and transfer to InvalidDistributionException
	}

public:
	/** Randomly picks n samples using a probability which is proportional to the sample score */
	TSampleSet DrawByScore(unsigned int n) const {
		std::vector<double> density = ComputeScoreDensity();
		TSampleSet picked;
		picked.Reserve(n);
		for(unsigned int i=0; i<n; ++i) {
			size_t p = RandomPickFromDensity(density);
			picked.Add(state(p), score(p));
		}
		return picked;
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
//		for(size_t i=0; i<Size(); ++i) {
//			this->set_state(i, space.random(state(i), noise));
//		}
	}

	/** Transforms all states by a function (invalidates scores) */
	template<class Function>
	void TransformStates(const Function& f) {
		std::transform(states_.begin(), states_.end(), states_.begin(), f);
//		for(size_t i=0; i<Size(); ++i) {
//			this->set_state(i, op(state(i)));
//		}
	}

	/** Transforms all scores with a function */
	template<class Function>
	void TransformScores(const Function& f) {
		std::transform(scores_.begin(), scores_.end(), scores_.begin(), f);
//		for(size_t i=0; i<Size(); ++i) {
//			this->set_score(i, mapper(score(i)));
//		}
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

	/** Computes scores for all states using an evaluation functin */
	template<class Function>
	void EvaluateAll(const Function& f) {
		scores_ = f(states());
	}

	/** Finds the best score */
	template<class CMP>
	Score FindBestScore() const {
		if(Size() == 0) {
			throw std::runtime_error("Can not get score of best sample for an empty sample set!");
		}
		typename std::vector<Score>::const_iterator it_best = std::min_element(scores_.begin(), scores_.end(), CMP());
		return *it_best;
	}

	/** Finds the best sample */
	template<class CMP>
	Sample FindBestSample() const {
		if(Size() == 0) {
			throw std::runtime_error("Can not get best sample for an empty sample set!");
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
		for(typename std::vector<IndexAndScore>::const_iterator it=v.begin(); it!=v.end(); ++it) {
			samples.Add(state(it->index_), it->score_);
		}
		return samples;
	}

	/** Prints all samples as a list */
	void print(std::ostream& os) const {
		os << "[Sample Set = {";
		for(size_t i=0; i<Size(); i++) {
			os << CreateSample(i) << ", ";
		}
		os << "}]";
	}

	/** Prints all samples, one line per sample */
	void printInLines(std::ostream& os) const {
		os << "[Sample Set = {";
		for(size_t i=0; i<Size(); i++) {
			os << "\t" << CreateSample(i) << "\n";
		}
		os << "}]";
	}

private:
	// list of states
	std::vector<State> states_;

	// list of scores
	std::vector<Score> scores_;

};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
