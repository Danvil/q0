#ifndef QUESTZERO_SAMPLE
#define QUESTZERO_SAMPLE
//---------------------------------------------------------------------------
#include <Danvil/Print.h>
//---------------------------------------------------------------------------

#define C_UNKNOWN_SCORE -1

//---------------------------------------------------------------------------

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

//---------------------------------------------------------------------------

template<typename State>
bool operator<(const TSample<State>& a, const TSample<State>& b) {
	return a.isScoreKnown() && (b.isScoreUnknown() || a.score() < b.score());
}

//---------------------------------------------------------------------------
#endif
