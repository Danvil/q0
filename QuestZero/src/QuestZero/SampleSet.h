#ifndef QUESTZERO_SAMPLESET
#define QUESTZERO_SAMPLESET

#include <boost/foreach.hpp>
#include <vector>
#include <algorithm>

template<typename Traits>
class Sample
{
public:
	typedef typename Traits::State State;

private:
	State _state;
	double _score;
	bool _isKnown;

public:
	Sample() {}

	Sample(const State& state)
	: _state(state),
	  _isKnown(false) {
	}

	Sample(const State& state, double score)
	: _state(state),
	  _score(score),
	  _isKnown(true) {
	}

	const State& state() const {
		return _state;
	}

	double score() const {
		assert(_isKnown);
		return _score;
	}

	bool isScoreUnknown() const {
		return !_isKnown;
	}

	bool isScoreKnown() const {
		return _isKnown;
	}

	void setScore(double score) {
		_score = score;
		_isKnown = true;
	}

};

template<typename Traits>
bool operator<(const State<Traits>& a, const State<Traits>& b) {
	return a.isScoreKnown() && (b.isScoreUnknown() || a.score() < b.score());
}

template<typename Traits>
class SampleSet
{
public:
	typedef typename Traits::State State;
	typedef typename Traits::Sample Sample;
	typedef typename Traits::Function Function;

public:
	std::vector<Sample> _samples;

public:
	bool isEmpty() const {
		return count() == 0;
	}

	size_t count() const {
		return _samples.size();
	}

	void add(const std::vector<State>& states) {
		BOOST_FOREACH(const State& s, states) {
			_samples.push_back(Sample(state));
		}
	}

	void evaluateUnknown(PTR(Function) f) {
		BOOST_FOREACH(Sample& s, _samples) {
			if(!s.isScoreKnown()) {
				s.setScore((*f)(s.state()));
			}
		}
	}

	void evaluateAll(PTR(Function) f) {
		BOOST_FOREACH(Sample& s, _samples) {
			s.setScore((*f)(s.state()));
		}
	}

	const Sample& best() const {
		if(isEmpty()) {
			throw std::runtime_error("Can not get best sample of empty sample set!");
		}
		const Sample& best_sample = _samples[0];
		for(size_t i=0; i<_samples.size(); ++i) {
			const Sample& current = _samples[i];
			if(current.isScoreKnown()
				&& (best_sample.isScoreUnknown() || current.score() < best_sample.score())
			) {
				best_sample = current;
			}
		}
		if(best_sample.isScoreUnknown()) {
			throw std::runtime_error("Can not get best sample if all samples are unevaluated!");
		}
		return best_sample;
	}

	SampleSet best(size_t n) const {
		std::sort(_samples.begin(), _samples.end());
		return SampleSet(std::vector<Samples>(_samples.begin(), _samples.begin() + n));
	}

};

#endif

