#ifndef QUESTZERO_SAMPLE
#define QUESTZERO_SAMPLE
//---------------------------------------------------------------------------
#include <QuestZero/Common/IPrintable.h>
#include <boost/assert.hpp>
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

#define C_UNKNOWN_SCORE -1

//---------------------------------------------------------------------------

template<typename State, typename Score>
class TSample
: public IPrintable
{
private:
	State _state;
	Score _score;
	double _weight;
	bool _isKnown;

public:
	TSample() {}

	TSample(const State& state)
	: _state(state),
	  _score(C_UNKNOWN_SCORE),
	  _weight(0.0),
	  _isKnown(false) {
	}

	TSample(const State& state, Score score, double weight)
	: _state(state),
	  _score(score),
	  _weight(weight),
	  _isKnown(true) {
	}

	const State& state() const {
		return _state;
	}

	Score score() const {
		BOOST_ASSERT(_isKnown);
		return _score;
	}

	double weight() const {
		return _weight;
	}

//	bool isScoreUnknown() const {
//		return !_isKnown;
//	}
//
//	bool isScoreKnown() const {
//		return _isKnown;
//	}
//
	void setScore(Score score) {
		_score = score;
		_isKnown = true;
	}

	void setWeight(double weight) {
		_weight = weight;
		_isKnown = true;
	}

	void setState(const State& state) {
		_state = state;
		_score = C_UNKNOWN_SCORE;
		_weight = 1.0;
		_isKnown = false;
	}

	void print(std::ostream& os) const {
		//if(isScoreKnown()) {
			os << "[Sample: Score=" << _score << ", Weight=" << _weight << ", State=" << _state << "]";
		//} else {
		//	os << "[Sample: Score unknown, State=" << _state << "]";
		//}
	}

	static bool CompareByScore(const TSample<State,Score>& a, const TSample<State,Score>& b) {
		return a.isScoreKnown() && (b.isScoreUnknown() || a.score() < b.score());
	}

	static bool CompareByWeight(const TSample<State,Score>& a, const TSample<State,Score>& b) {
		return a.isScoreKnown() && (b.isScoreUnknown() || a.weight() < b.weight());
	}

};

//---------------------------------------------------------------------------

//template<typename State, typename Score>
//bool operator<(const TSample<State,Score>& a, const TSample<State,Score>& b) {
//	return a.isScoreKnown() && (b.isScoreUnknown() || a.score() < b.score());
//}

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
