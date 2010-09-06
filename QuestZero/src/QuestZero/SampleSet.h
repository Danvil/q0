#ifndef QUESTZERO_SAMPLESET
#define QUESTZERO_SAMPLESET

#include "IFunction.h"
#include <Danvil/Print.h>
#include <boost/foreach.hpp>
#include <vector>
#include <algorithm>
#include <ostream>

#define C_UNKNOWN_SCORE -1

template<typename State>
class TSample
: public Danvil::Print::IPrintable
{
private:
	State _state;
	double _score;
	bool _isKnown;

public:
	TSample() {}

	TSample(const State& state)
	: _state(state),
	  _score(C_UNKNOWN_SCORE),
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

template<typename State>
bool operator<(const TSample<State>& a, const TSample<State>& b) {
	return a.isScoreKnown() && (b.isScoreUnknown() || a.score() < b.score());
}

template<typename State>
class TSampleSet
: public Danvil::Print::IPrintable
{
public:
	typedef TSample<State> Sample;
	typedef IFunction<State> Function;

public:
	TSampleSet() { }

	TSampleSet(const std::vector<Sample>& samples)
	: _data(samples) { }

	TSampleSet(const std::vector<State>& states) {
		add(states);
	}

	~TSampleSet() {}

	bool isEmpty() const {
		return count() == 0;
	}

	size_t count() const {
		return _data.count();
	}

	void add(const State& state) {
		_data.add(state);
	}

	void add(const std::vector<State>& states) {
		_data.add(states);
	}

	const std::vector<Sample>& samples() const {
		return _data.samples();
	}

	const std::vector<State>& states() const {
		return _data.states();
	}

	const std::vector<double>& scores() const {
		return _data.scores();
	}

	/** Evaluates only unknown samples */
	void evaluateUnknown(PTR(Function) f) {
		std::vector<size_t> unknown_ids;
		std::vector<State> unknown_states = _data.statesWithUnknownScore(unknown_ids);
		std::vector<double> scores = f->evaluateParallel(unknown_states);
		_data.setScores(scores, unknown_ids);
	}

	/** Evaluates all samples (also if the sample score is already known) */
	void evaluateAll(PTR(Function) f) {
		std::vector<double> scores = f->evaluateParallel(_data.states());
		_data.setScores(scores);
	}

	const Sample& best() const {
		return _data.best();
	}

	TSampleSet best(size_t n) const {
		return TSampleSet(_data.best(n));
	}

	void print(std::ostream& os) const {
		os << "[Sample Set = {";
		BOOST_FOREACH(const Sample& s, _data.samples()) {
			os << s << ", ";
		}
		os << "}]";
	}

private:
	/**
	 * - Functions 'states()' and 'scores()' copies states/numbers
	 * - Function 'best(int)' copies all samples and performs a sort
	 */
	class SampleSetDataSimple
	{
	public:
		SampleSetDataSimple() {}

		SampleSetDataSimple(const std::vector<Sample>& samples)
		: _samples(samples) { }

		~SampleSetDataSimple() {}

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

/*	class SampleSetDataCached
	{
	public:
		SampleSetDataCached() {}

		SampleSetDataCached(const std::vector<Sample>& samples) {
			add(samples);
		}

		~SampleSetDataCached() {}

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
	};*/

private:
	SampleSetDataSimple _data;

};

#endif

