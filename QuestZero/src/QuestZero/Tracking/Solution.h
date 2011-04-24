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
#include "QuestZero/Tracking/TimeRange.h"
#include <Danvil/Print.h>
#include <vector>
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

template<typename Time, typename State, typename Score>
struct TSolution
: public Danvil::Print::IPrintable
{
	struct TimeRangeException {};

	struct UnkownScoreException {};

	struct Item
	{
		Item()
		: isEvaluated(false)
		{}

		size_t index;
		Time time;
		State state;
		Score score;
		bool isEvaluated;
	};

	typedef typename std::vector<Item> ItemContainer;

	typedef typename ItemContainer::const_iterator ConstIt;

	typedef typename ItemContainer::iterator It;

	TSolution(size_t count = 0) {
		this->Initialize(count);
	}

	template<typename I2T>
	TSolution(size_t count, const I2T& op) {
		this->Initialize(count);
		this->SetTimes(op);
	}

	const std::vector<Item>& GetItems() const {
		return items_;
	}

	bool IsEmpty() const {
		return items_.size() == 0;
	}

	const Item& GetItemByIndex(size_t i) const {
		if(i >= items_.size()) {
			throw TimeRangeException();
		}
		return items_[i];
	}

	Item& GetItemByIndex(size_t i) {
		if(i >= items_.size()) {
			throw TimeRangeException();
		}
		return items_[i];
	}

	const Item& GetItemByTime(Time t) const {
		for(ConstIt it=items_.begin(); it!=items_.end(); ++it) {
			if(it->time == t) {
				return *it;
			}
		}
		throw TimeRangeException();
	}

	It GetFirstUnknown() {
		for(It it=items_.begin(); it!=items_.end(); ++it) {
			if(!it->isEvaluated) {
				return it;
			}
		}
		throw TimeRangeException();

	}

	ConstIt GetFirstUnknown() const {
		for(ConstIt it=items_.begin(); it!=items_.end(); ++it) {
			if(!it->isEvaluated) {
				return it;
			}
		}
		throw TimeRangeException();
	}

	It GetBegin() {
		return items_.begin();
	}

	ConstIt GetBegin() const {
		return items_.begin();
	}

	It GetEnd() {
		return items_.end();
	}

	ConstIt GetEnd() const {
		return items_.end();
	}

	void Set(size_t index, const State& state, Score score) {
		Item& a = GetItemByIndex(index);
		a.state = state;
		a.score = score;
		a.isEvaluated = true;
	}

	void Set(size_t index, const TSample<State, Score>& sample) {
		Set(index, sample.state(), sample.score());
	}

	const State& GetState(size_t i) const {
		return GetItemByIndex(i).state;
	}

	Score GetScore(size_t i) const {
		return GetItemByIndex(i).score;
	}

	bool IsEvaluated(size_t i) const {
		return GetItemByIndex(i).isEvaluated;
	}

	bool AreAllEvaluated() const {
		for(ConstIt it=items_.begin(); it!=items_.end(); ++it) {
			if(!it->isEvaluated) {
				return false;
			}
		}
		return true;
	}

	double ComputeTotalScore() const {
		double sum = 0;
		for(ConstIt it=items_.begin(); it!=items_.end(); ++it) {
			if(!it->isEvaluated) {
				sum += it->score;
			}
		}
		return sum;
	}

	double ComputeMeanScore() const {
		double sum = 0;
		size_t n = 0;
		for(ConstIt it=items_.begin(); it!=items_.end(); ++it) {
			if(!it->isEvaluated) {
				sum += it->score;
				n ++;
			}
		}
		return sum / double(n);
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
		for(ConstIt it=items_.begin(); it!=items_.end(); ++it) {
			if(it->isEvaluated) {
				scores.push_back(it->score);
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
		for(ConstIt it=items_.begin(); it!=items_.end(); ++it) {
			if(it->isEvaluated) {
				states.push_back(it->state);
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
		for(ConstIt it=items_.begin(); it!=items_.end(); ++it) {
			const Item& i = *it;
			os << "\n";
			if(i.isEvaluated) {
				os << i.index << "\tTime=" << i.time << "\tScore=" << i.score << "\tState=" << i.state;
			}
			else {
				os << i.index << "\tTime=" << i.time << "\tScore=unknown\tState=---";
			}
		}
		os << "\n]";
	}

private:
	void Initialize(size_t count) {
		items_.resize(count);
		size_t i = 0;
		for(It it=items_.begin(); it!=items_.end(); ++it, i++) {
			it->index = i;
			it->isEvaluated = false;
		}
	}

	template<typename I2T>
	void SetTimes(const I2T& op) {
		for(It it=items_.begin(); it!=items_.end(); ++it) {
			it->time = op(it->index);
		}
	}

private:
	std::vector<Item> items_;
};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
