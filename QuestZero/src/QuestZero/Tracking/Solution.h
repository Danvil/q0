/*
 * Solution.h
 *
 *  Created on: Sep 12, 2010
 *      Author: david
 */

#ifndef SOLUTION_H_
#define SOLUTION_H_
//---------------------------------------------------------------------------
#include "QuestZero/Common/Sample.h"
#include "TimeRange.h"
#include <Danvil/Print.h>
#include <boost/foreach.hpp>
#include <vector>
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

template<typename Time, typename State, typename Score>
struct TSolution
: public Danvil::Print::IPrintable
{
	typedef TTimeRange<Time> TimeRange;

	struct TimeRangeException {};

	struct UnkownScoreException {};

	struct Item
	{
		Item()
		: isEvaluated(false)
		{}

		Time time;
		State state;
		Score score;
		bool isEvaluated;
	};

	TSolution() {}

	TSolution(const TimeRange& range)
	: range_(range) {
		items_.resize(range_.frameCount());
		for(size_t i=0; i<items_.size(); ++i) {
			items_[i].time = range.indexToTime(i);
		}
	}

	const std::vector<Item>& items() const {
		return items_;
	}

	bool IsEmpty() const {
		return items_.size() == 0;
	}

	void set(const Time& t, const State& state, const Score& score) {
		unsigned int index = range_.timeToIndex(t);
		Item& i = items_[index];
		i.state = state;
		i.score = score;
		i.isEvaluated = true;
	}

	void set(const Time& t, const TSample<State, Score>& sample) {
		set(t, sample.state(), sample.score());
	}

	const State& state(const Time& t) const {
		return itemByTime(t).state;
	}

	const Score& score(const Time& t) const {
		return itemByTime(t).score;
	}

	bool IsEvaluated(const Time& t) const {
		return itemByTime(t).isEvaluated;
	}

	bool AreAllEvaluated() const {
		BOOST_FOREACH(const Item& i, items_) {
			if(!i.isEvaluated) {
				return false;
			}
		}
		return true;
	}

	double ComputeTotalScore() const {
		double sum = 0;
		BOOST_FOREACH(const Item& i, items_) {
			sum += i.score;
		}
		return sum;
	}

	double ComputeMeanScore() const {
		return ComputeTotalScore() / (double)items_.size();
	}

	double GetInitialScore() const {
		if(items_.size() == 0) {
			throw TimeRangeException();
		}
		const Item& i = items_[0];
		if(!i.isEvaluated) {
			throw UnkownScoreException();
		}
		return i.score;
	}

	double GetFinalScore() const {
		if(items_.size() == 0) {
			throw TimeRangeException();
		}
		const Item& i = items_[items_.size() - 1];
		if(!i.isEvaluated) {
			throw UnkownScoreException();
		}
		return i.score;
	}

	std::vector<Score> CreateScoreList(bool throw_if_unknown=true) const {
		std::vector<Score> scores;
		scores.reserve(items_.size());
		for(size_t k=0; k<items_.size(); ++k) {
			const Item& i = items_[k];
			if(i.isEvaluated) {
				scores.push_back(i.score);
			} else {
				if(throw_if_unknown) {
					throw UnkownScoreException();
				}
			}
		}
		return scores;
	}

	std::vector<State> CreateStateList(bool throw_if_unknown=true) const {
		std::vector<State> states;
		states.reserve(items_.size());
		for(size_t k=0; k<items_.size(); ++k) {
			const Item& i = items_[k];
			if(i.isEvaluated) {
				states.push_back(i.state);
			} else {
				if(throw_if_unknown) {
					throw UnkownScoreException();
				}
			}
		}
		return states;
	}

	void print(std::ostream& os) const {
		os << "Solution=[";
		for(size_t k=0; k<items_.size(); ++k) {
			const Item& i = items_[k];
			os << i.time << ": " << "Score=" << i.score << ", State=" << i.state;
			if(k + 1 < items_.size()) {
				os << ", ";
			}
		}
		os << "]";
	}

private:
	const Item& itemByTime(const Time& t) const {
		return items_[range_.timeToIndex(t)];
	}

	TimeRange range_;

	std::vector<Item> items_;
};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
