#ifndef QUESTZERO_SAMPLESET
#define QUESTZERO_SAMPLESET
//---------------------------------------------------------------------------
#include "Sample.h"
#include <Danvil/Print.h>
#include <Danvil/Ptr.h>
#include <boost/foreach.hpp>
#include <vector>
#include <algorithm>
#include <ostream>
//---------------------------------------------------------------------------

/**
 * - Functions 'states()' and 'scores()' copies states/numbers
 * - Function 'best(int)' copies all samples and performs a sort
 */
template<typename State>
class SampleDataStorage
{
public:
	typedef TSample<State> Sample;

public:
	SampleDataStorage() {}

	SampleDataStorage(const std::vector<Sample>& samples)
	: _samples(samples) { }

	~SampleDataStorage() {}

	size_t count() const {
		return _samples.size();
	}

	void add(const Sample& sample) {
		_samples.push_back(sample);
	}

	void add(const std::vector<Sample>& samples) {
		BOOST_FOREACH(const Sample& s, samples) {
			add(s);
		}
	}

	void add(const State& state) {
		_samples.push_back(Sample(state));
	}

	void add(const std::vector<State>& states) {
		BOOST_FOREACH(const State& s, states) {
			add(s);
		}
	}

	const std::vector<Sample>& samples() const {
		return _samples;
	}

	const std::vector<State>& states() const {
		std::vector<State> r;
		r.reserve(_samples.size());
		BOOST_FOREACH(const Sample& s, _samples) {
			r.push_back(s.state());
		}
		return r;
	}

	std::vector<State> statesWithUnknownScore(std::vector<size_t>& indices) const {
		indices.clear();
		std::vector<State> unknown_states;
		indices.reserve(count());
		unknown_states.reserve(count());
		for(size_t i=0; i<count(); i++) {
			if(_samples[i].isScoreUnknown()) {
				indices.push_back(i);
				unknown_states.push_back(_samples[i].state());
			}
		}
		return unknown_states;
	}

	const std::vector<double>& scores() const {
		std::vector<double> r;
		r.reserve(_samples.size());
		BOOST_FOREACH(const Sample& s, _samples) {
			r.push_back(s.score());
		}
		return r;
	}

	void setScore(size_t i, double score) {
		assert(i < _samples.size());
		_samples[i].setScore(score);
	}

	void setScores(const std::vector<double>& scores) {
		assert(scores.size() == _samples.size());
		for(size_t i=0; i<_samples.size(); i++) {
			setScore(i, scores[i]);
		}
	}

	void setScores(const std::vector<double>& scores, const std::vector<size_t>& indices) {
		assert(scores.size() == indices.size());
		for(size_t i=0; i<scores.size(); i++) {
			setScore(indices[i], scores[i]);
		}
	}

	const Sample& best() const {
		if(_samples.size() == 0) {
			throw std::runtime_error("Can not get best sample of empty sample set!");
		}
		const Sample* best_sample = &(_samples[0]);
		for(size_t i=0; i<_samples.size(); ++i) {
			const Sample* current = &(_samples[i]);
			if(current->isScoreKnown()
				&& (best_sample->isScoreUnknown() || current->score() < best_sample->score())
			) {
				best_sample = current;
			}
		}
		if(best_sample->isScoreUnknown()) {
			throw std::runtime_error("Can not get best sample if all samples are unevaluated!");
		}
		return *best_sample;
	}

	std::vector<Sample> best(size_t n) const {
		std::vector<Sample> temp = _samples;
		std::sort(temp.begin(), temp.end());
		return std::vector<Sample>(temp.begin(), temp.begin() + n);
	}

private:
	std::vector<Sample> _samples;
};

//---------------------------------------------------------------------------

template<typename State>
class CachedSamplesDataStorage
{
public:
	typedef TSample<State> Sample;

public:
	CachedSamplesDataStorage() {}

	CachedSamplesDataStorage(const std::vector<Sample>& samples) {
		add(samples);
	}

	~CachedSamplesDataStorage() {}

	size_t count() const {
		return _samples.size();
	}

	void add(const Sample& sample) {
		_samples.push_back(sample);
		_states.push_back(sample.state());
		if(sample.isScoreKnown()) {
			_scores.push_back(sample.score());
		} else {
			_scores.push_back(C_UNKNOWN_SCORE);
		}
	}

	void add(const std::vector<Sample>& samples) {
		BOOST_FOREACH(const Sample& s, samples) {
			add(s);
		}
	}

	void add(const State& state) {
		_samples.push_back(Sample(state));
		_states.push_back(state);
		_scores.push_back(C_UNKNOWN_SCORE);
	}

	void add(const std::vector<State>& states) {
		BOOST_FOREACH(const State& s, states) {
			add(s);
		}
	}

	const std::vector<Sample>& samples() const { return _samples; }

	const std::vector<State>& states() const { return _states; }

	const std::vector<double>& scores() const { return _scores; }

	std::vector<State> statesWithUnknownScore(std::vector<size_t>& indices) const {
		indices.clear();
		std::vector<State> unknown_states;
		indices.reserve(count());
		unknown_states.reserve(count());
		for(size_t i=0; i<count(); i++) {
			if(_samples[i].isScoreUnknown()) {
				indices.push_back(i);
				unknown_states.push_back(_samples[i].state());
			}
		}
		return unknown_states;
	}

	void setScore(size_t i, double score) {
		assert(i < _samples.size());
		_samples[i].setScore(score);
		_scores[i] = score;
	}

	void setScores(const std::vector<double>& scores) {
		assert(scores.size() == _samples.size());
		for(size_t i=0; i<_samples.size(); i++) {
			setScore(i, scores[i]);
		}
	}

	void setScores(const std::vector<double>& scores, const std::vector<size_t>& indices) {
		assert(scores.size() == indices.size());
		for(size_t i=0; i<count(); i++) {
			setScore(indices[i], scores[i]);
		}
	}

	const Sample& best() const {
		if(_samples.size() == 0) {
			throw std::runtime_error("Can not get best sample of empty sample set!");
		}
		const Sample* best_sample = &(_samples[0]);
		for(size_t i=0; i<_samples.size(); ++i) {
			const Sample* current = &(_samples[i]);
			if(current->isScoreKnown()
				&& (best_sample->isScoreUnknown() || current->score() < best_sample->score())
			) {
				best_sample = current;
			}
		}
		if(best_sample->isScoreUnknown()) {
			throw std::runtime_error("Can not get best sample if all samples are unevaluated!");
		}
		return *best_sample;
	}

	std::vector<Sample> best(size_t n) const {
		std::vector<Sample> temp = _samples;
		std::sort(temp.begin(), temp.end());
		return std::vector<Sample>(temp.begin(), temp.begin() + n);
	}

private:
	std::vector<State> _states;
	std::vector<double> _scores;
	std::vector<Sample> _samples;
};

//---------------------------------------------------------------------------

template<typename State, class DataStorage = SampleDataStorage<State> >
class TSampleSet
: public DataStorage,
  public Danvil::Print::IPrintable
{
public:
	typedef TSample<State> Sample;

public:
	TSampleSet() { }

	TSampleSet(const std::vector<Sample>& samples)
	: DataStorage(samples) { }

	TSampleSet(const std::vector<State>& states) {
		add(states);
	}

	~TSampleSet() {}

	bool isEmpty() const {
		return this->count() == 0;
	}

	/** Evaluates only unknown samples */
	template<class Function>
	void evaluateUnknown(const Function& f) {
		std::vector<size_t> unknown_ids;
		std::vector<State> unknown_states = this->statesWithUnknownScore(unknown_ids);
		std::vector<double> scores = f(unknown_states);
		this->setScores(scores, unknown_ids);
	}

	/** Evaluates all samples (also if the sample score is already known) */
	template<class Function>
	void evaluateAll(const Function& f) {
		std::vector<double> scores = f(this->states());
		this->setScores(scores);
	}

	TSampleSet bestAsSet(size_t n) const {
		return TSampleSet(this->best(n));
	}

	void print(std::ostream& os) const {
		os << "[Sample Set = {";
		BOOST_FOREACH(const Sample& s, this->samples()) {
			os << s << ", ";
		}
		os << "}]";
	}

};

//---------------------------------------------------------------------------
#endif
