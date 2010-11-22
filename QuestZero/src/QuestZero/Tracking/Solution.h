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

template<typename Time, typename State>
struct TSolution
: public Danvil::Print::IPrintable
{
	typedef TTimeRange<Time> TimeRange;

	struct TimeRangeException {};

	struct UnkownScoreException {};

	TSolution(const TimeRange& range)
	: range_(range) {
		items_.resize(range_.frameCount());
		for(size_t i=0; i<items_.size(); ++i) {
			items_[i].time = range.timeToIndex(i);
		}
	}

	void set(const Time& t, const TSample<State>& sample) {
		unsigned int index = range_.timeToIndex(t);
		Item& i = items_[index];
		i.state = sample.state();
		i.score = sample.score();
		i.isEvaluated = true;
	}

	const State& state(const Time& t) const {
		return itemByTime(t).state;
	}

	const double& score(const Time& t) const {
		return itemByTime(t).score;
	}

	bool areAllEvaluated() const {
		BOOST_FOREACH(const Item& i, items_) {
			if(!i.isEvaluated) {
				return false;
			}
		}
		return true;
	}

	double totalScore() const {
		double sum = 0;
		BOOST_FOREACH(const Item& i, items_) {
			sum += i.score;
		}
		return sum;
	}

	double meanScore() const {
		return totalScore() / (double)items_.size();
	}

	double initialScore() const {
		if(items_.size() == 0) {
			throw TimeRangeException();
		}
		const Item& i = items_[0];
		if(!i.isEvaluated) {
			throw UnkownScoreException();
		}
		return i.score;
	}

	double finalScore() const {
		if(items_.size() == 0) {
			throw TimeRangeException();
		}
		const Item& i = items_[items_.size() - 1];
		if(!i.isEvaluated) {
			throw UnkownScoreException();
		}
		return i.score;
	}

	std::vector<double> scoreList() const {
		std::vector<double> scores;
		scores.reserve(items_.size());
		for(size_t k=0; k<items_.size(); ++k) {
			const Item& i = items_[k];
			if(!i.isEvaluated) {
				throw UnkownScoreException();
			}
			scores.push_back(i.score);
		}
		return scores;
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
	struct Item
	{
		Item()
		: isEvaluated(false)
		{}

		Time time;
		State state;
		double score;
		bool isEvaluated;
	};

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
