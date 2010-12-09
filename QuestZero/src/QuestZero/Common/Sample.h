#ifndef QUESTZERO_SAMPLE
#define QUESTZERO_SAMPLE
//---------------------------------------------------------------------------
#include <Danvil/Print.h>
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

#define C_UNKNOWN_SCORE -1

//---------------------------------------------------------------------------

template<typename State, typename Score>
class TSample
: public Danvil::Print::IPrintable
{
private:
	State _state;
	Score _score;
	bool _isKnown;

public:
	TSample() {}

	TSample(const State& state)
	: _state(state),
	  _score(C_UNKNOWN_SCORE),
	  _isKnown(false) {
	}

	TSample(const State& state, Score score)
	: _state(state),
	  _score(score),
	  _isKnown(true) {
	}

	TSample(const TSample& sample)
	: _state(sample._state),
	  _score(sample._score),
	  _isKnown(sample._isKnown) {
	}

	TSample& operator=(const TSample& sample) {
		_state = sample._state;
		_score = sample._score;
		_isKnown = sample._isKnown;
		return *this;
	}

	const State& state() const {
		return _state;
	}

	Score score() const {
		assert(_isKnown);
		return _score;
	}

	bool isScoreUnknown() const {
		return !_isKnown;
	}

	bool isScoreKnown() const {
		return _isKnown;
	}

	void setScore(Score score) {
		_score = score;
		_isKnown = true;
	}

	void setState(const State& state) {
		_state = state;
		_score = C_UNKNOWN_SCORE;
		_isKnown = false;
	}

	void print(std::ostream& os) const {
		if(isScoreKnown()) {
			os << "[Sample: Score=" << _score << ", State=" << _state << "]";
		} else {
			os << "[Sample: Score unknown, State=" << _state << "]";
		}
	}
};

//---------------------------------------------------------------------------

template<typename State, typename Score>
bool operator<(const TSample<State,Score>& a, const TSample<State,Score>& b) {
	return a.isScoreKnown() && (b.isScoreUnknown() || a.score() < b.score());
}

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
