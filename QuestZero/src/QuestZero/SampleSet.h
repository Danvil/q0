#ifndef QUESTZERO_SAMPLESET
#define QUESTZERO_SAMPLESET

#include <Danvil/Print.h>
#include <boost/foreach.hpp>
#include <vector>
#include <algorithm>
#include <ostream>

template<typename Traits>
class TSample
: public Danvil::Print::IPrintable
{
public:
	typedef typename Traits::State State;

private:
	State _state;
	double _score;
	bool _isKnown;

public:
	TSample() {}

	TSample(const State& state)
	: _state(state),
	  _isKnown(false) {
	}

	TSample(const State& state, double score)
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

	void print(std::ostream& os) const {
		if(isScoreKnown()) {
			os << "[Sample: Score=" << _score << ", State=" << _state << "]";
		} else {
			os << "[Sample: Score unknown, State=" << _state << "]";
		}
	}
};

template<typename Traits>
bool operator<(const TSample<Traits>& a, const TSample<Traits>& b) {
	return a.isScoreKnown() && (b.isScoreUnknown() || a.score() < b.score());
}

template<typename Traits>
class TSampleSet
: public Danvil::Print::IPrintable
{
public:
	typedef typename Traits::State State;
	typedef typename Traits::Sample Sample;
	typedef typename Traits::Function Function;

public:
	std::vector<Sample> _samples;

public:
	TSampleSet() {
	}

	TSampleSet(const std::vector<Sample>& samples) {
		_samples = samples;
	}

	TSampleSet(const std::vector<State>& states) {
		add(states);
	}

	bool isEmpty() const {
		return count() == 0;
	}

	size_t count() const {
		return _samples.size();
	}

	void add(const State& state) {
		_samples.push_back(Sample(state));
	}

	void add(const std::vector<State>& states) {
		BOOST_FOREACH(const State& s, states) {
			_samples.push_back(Sample(s));
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

	TSampleSet best(size_t n) {
		std::sort(_samples.begin(), _samples.end());
		return TSampleSet(std::vector<Sample>(_samples.begin(), _samples.begin() + n));
	}

	void print(std::ostream& os) const {
		os << "[Sample Set = {";
		BOOST_FOREACH(const Sample& s, _samples) {
			os << s << ", ";
		}
		os << "}]";
	}

};

#endif

