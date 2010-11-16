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

/**
 * - Functions 'states()' and 'scores()' copies states/numbers
 * - Function 'best(int)' copies all samples and performs a sort
 */
template<typename State>
struct SampleDataStorage
{
	typedef TSample<State> Sample;

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

	const Sample& operator[](size_t i) const {
		return _samples[i];
	}

	Sample& operator[](size_t i) {
		return _samples[i];
	}

	const Sample& sample(size_t i) const {
		return _samples[i];
	}

	double score(size_t i) const {
		return _samples[i].score();
	}

	const State& state(size_t i) const {
		return _samples[i].state();
	}

	const std::vector<Sample>& samples() const { return _samples; }

	std::vector<Sample>& samples() { return _samples; }

	std::vector<State> states() const {
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

	std::vector<double> scores() const {
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

private:
	std::vector<Sample> _samples;
};

//---------------------------------------------------------------------------

template<typename State>
struct CachedSamplesDataStorage
{
	typedef TSample<State> Sample;

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

	const Sample& operator[](size_t i) const {
		return _samples[i];
	}

	Sample& operator[](size_t i) {
		return _samples[i];
	}

	const Sample& sample(size_t i) const {
		return _samples[i];
	}

	double score(size_t i) const {
		return _scores[i];
	}

	const State& state(size_t i) const {
		return _states[i];
	}

	const std::vector<Sample>& samples() const { return _samples; }

	std::vector<Sample>& samples() { return _samples; }

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

private:
	std::vector<State> _states;
	std::vector<double> _scores;
	std::vector<Sample> _samples;
};

//---------------------------------------------------------------------------

template<typename State, class DataStorage = SampleDataStorage<State> >
struct TSampleSet
: public DataStorage,
  public Danvil::Print::IPrintable
{
	struct CanNotNormalizeZeroListException {};

	struct InvalidDistributionException {};

	typedef TSample<State> Sample;

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

private:
	/** Computes the density of the probability distribution got by normalizing the samples scores */
	std::vector<double> scoreDensity() const {
		if(this->count() == 0) {
			throw CanNotNormalizeZeroListException();
		}
		// we first build the density using the unnormalized scores
		std::vector<double> d;
		d.reserve(this->count());
		double last = 0;
		for(size_t i=0; i<this->count(); ++i) {
			last += (*this)[i].score();
			d.push_back(last);
		}
		// last is now the total sum of all scores
		// using this we perform the normalization
		if(last == 0) {
			throw CanNotNormalizeZeroListException();
		}
		double scl = 1.0 / last;
		for(size_t i=0; i<d.size(); ++i) {
			d[i] *= scl;
		}
		return d;
	}

	/** Finds the index of the smallest value larger than the given value */
	static size_t FindSmallestLargerThan(const std::vector<double>& list, double val) {
		// FIXME use bisection search here!
		for(size_t i=0; i<list.size(); ++i) {
			if(val <= list[i]) {
				return i;
			}
		}
		throw InvalidDistributionException(); // TODO throw a different exception here!
	}

	/** Randomly picks a value from a discreet probability distribution */
	static size_t RandomPickFromDensity(const std::vector<double>& density) {
		double r = RandomNumbers::Uniform<double>();
		return FindSmallestLargerThan(density, r);
		// TODO catch possible exceptions and transfer to InvalidDistributionException
	}

public:
	/** Randomly picks samples using a probability which is proportional to the sample score */
	TSampleSet drawByScore(unsigned int n) const {
		std::vector<double> density = scoreDensity();
		TSampleSet picked;
		for(unsigned int i=0; i<n; ++i) {
			size_t p = RandomPickFromDensity(density);
			picked.add((*this)[p]);
		}
		return picked;
	}

	/** Adds noise to all states invalidating the scores */
	template<class Space>
	void addNoise(const Space& space, const std::vector<double>& noise) {
		for(size_t i=0; i<this->count(); ++i) {
			Sample& sample = (*this)[i];
			sample.setState(space.random(sample.state(), noise));
		}
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

	template<class CMP>
	const Sample& best() const {
		CMP cmp;
		if(this->count() == 0) {
			throw std::runtime_error("Can not get best sample of empty sample set!");
		}
		const Sample* best_sample = &(this->sample(0));
		for(size_t i=0; i<this->count(); ++i) {
			const Sample* current = &(this->sample(i));
			if(current->isScoreKnown()
				&& (best_sample->isScoreUnknown() || cmp(current->score(), best_sample->score()))
			) {
				best_sample = current;
			}
		}
		if(best_sample->isScoreUnknown()) {
			throw std::runtime_error("Can not get best sample if all samples are unevaluated!");
		}
		return *best_sample;
	}

	template<class CMP>
	std::vector<Sample> best(size_t n) const {
		std::vector<Sample> temp = this->samples();
		std::sort(temp.begin(), temp.end(), CMP());
		return std::vector<Sample>(temp.begin(), temp.begin() + n);
	}

	template<class CMP>
	TSampleSet bestAsSet(size_t n) const {
		return TSampleSet(this->template best<CMP>(n));
	}

	template<class ScoreMapper>
	void mapScores(const ScoreMapper& mapper) {
		for(size_t i=0; i<this->count(); i++) {
			this->setScore(i, mapper(this->score(i)));
		}
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
