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
	struct EmptySolutionException {};

	struct InvalidTimeException {};

	struct StateNotKnownException {};

	struct AllStatesNotKnownException {};

	struct ScoreNotEvaluatedException {};

	struct Item
	{
		Item()
		: is_known_(false), is_evaluated_(false)
		{}

//		size_t GetIndex() const {
//			return index_;
//		}
//
//		Time GetTime() const {
//			return time_;
//		}
//
//		bool IsKnown() const {
//			return is_known_;
//		}
//
//		const State& GetState() const {
//			return state_;
//		}
//
//		bool IsEvaluated() const {
//			return is_evaluated_;
//		}
//
//		Score GetScore() const {
//			return score_;
//		}

	private:
		size_t index_;
		Time time_;
		bool is_known_;
		State state_;
		bool is_evaluated_;
		Score score_;

		friend struct TSolution;
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

	bool IsEmpty() const {
		return items_.size() == 0;
	}

	size_t Count() const {
		return items_.size();
	}

//	const std::vector<Item>& GetItems() const {
//		return items_;
//	}

//	const Item& GetItemByIndex(size_t i) const {
//		return items_[i];
//	}
//
//	Item& GetItemByIndex(size_t i) {
//		return items_[i];
//	}

	size_t GetIndexByTime(Time t) const {
		for(ConstIt it=items_.begin(); it!=items_.end(); ++it) {
			if(it->time_ == t) {
				return it->index_;
			}
		}
		throw InvalidTimeException();
	}

	/** Gets the index of the next unknown time tick */
	size_t SearchNextUnknown(size_t start) const {
		for(ConstIt it=items_.begin()+start; it<items_.end(); ++it) {
			if(!it->is_known_) {
				return it->index_;
			}
		}
		return items_.size();
	}

	/** Gets the index of the next known time tick */
	size_t SearchNextKnown(size_t start) const {
		for(ConstIt it=items_.begin()+start; it<items_.end(); ++it) {
			if(it->is_known_) {
				return it->index_;
			}
		}
		return items_.size();
	}

	/** Gets the index of the previous unknown time tick */
	size_t SearchPreviousKnown(size_t start) const {
		for(ConstIt it=items_.begin()+start-1; it>=items_.begin(); --it) {
			if(!it->is_known_) {
				return it->index_;
			}
		}
		return items_.size();
	}

//	It GetBegin() {
//		return items_.begin();
//	}
//
//	ConstIt GetBegin() const {
//		return items_.begin();
//	}
//
//	It GetEnd() {
//		return items_.end();
//	}
//
//	ConstIt GetEnd() const {
//		return items_.end();
//	}

	void Set(size_t index, const State& state) {
		Item& a = items_[index];
		a.state_ = state;
		a.is_known_ = true;
		a.score_ = Score(0);
		a.is_evaluated_ = false;
	}

	void Set(size_t index, const State& state, Score score) {
		Item& a = items_[index];
		a.state_ = state;
		a.is_known_ = true;
		a.score_ = score;
		a.is_evaluated_ = true;
	}

	void Set(size_t index, Time time, const State& state) {
		Item& a = items_[index];
		a.time_ = time;
		a.state_ = state;
		a.is_known_ = true;
		a.score_ = Score(0);
		a.is_evaluated_ = false;
	}

	void Set(size_t index, Time time, const State& state, Score score) {
		Item& a = items_[index];
		a.time_ = time;
		a.state_ = state;
		a.is_known_ = true;
		a.score_ = score;
		a.is_evaluated_ = true;
	}

	void Set(size_t index, const TSample<State, Score>& sample) {
		Set(index, sample.state(), sample.score());
	}

	Time GetTime(size_t i) const {
		return items_[i].time_;
	}

	bool IsKnown(size_t i) const {
		return items_[i].is_known_;
	}

	const State& GetState(size_t i) const {
		if(!items_[i].is_known_) {
			throw StateNotKnownException();
		}
		return items_[i].state_;
	}

	bool IsEvaluated(size_t i) const {
		return items_[i].is_evaluated_;
	}

	Score GetScore(size_t i) const {
		if(!items_[i].is_evaluated_) {
			throw ScoreNotEvaluatedException();
		}
		return items_[i].score_;
	}

	bool AreAllKnown() const {
		for(ConstIt it=items_.begin(); it!=items_.end(); ++it) {
			if(!it->IsKnown()) {
				return false;
			}
		}
		return true;
	}

	bool AreAllEvaluated() const {
		for(ConstIt it=items_.begin(); it!=items_.end(); ++it) {
			if(!it->IsEvaluated()) {
				return false;
			}
		}
		return true;
	}

	double ComputeTotalScore() const {
		if(items_.size() == 0) {
			throw EmptySolutionException();
		}
		double sum = 0;
		for(ConstIt it=items_.begin(); it!=items_.end(); ++it) {
			if(!it->IsEvaluated()) {
				sum += it->GetScore();
			}
		}
		return sum;
	}

	double ComputeMeanScore() const {
		if(items_.size() == 0) {
			throw EmptySolutionException();
		}
		double sum = 0;
		size_t n = 0;
		for(ConstIt it=items_.begin(); it!=items_.end(); ++it) {
			if(!it->IsEvaluated()) {
				sum += it->GetScore();
				n ++;
			}
		}
		return sum / double(n);
	}

	double GetInitialScore() const {
		if(items_.size() == 0) {
			throw EmptySolutionException();
		}
		return GetScore(0);
	}

	double GetFinalScore() const {
		if(items_.size() == 0) {
			throw EmptySolutionException();
		}
		return GetScore(items_.size() - 1);
	}

	std::vector<Score> CreateScoreList(bool throw_if_unknown=true) const {
		std::vector<Score> scores;
		scores.reserve(items_.size());
		for(ConstIt it=items_.begin(); it!=items_.end(); ++it) {
			if(it->IsEvaluated()) {
				scores.push_back(it->GetScore());
			} else {
				if(throw_if_unknown) {
					throw ScoreNotEvaluatedException();
				}
			}
		}
		return scores;
	}

	std::vector<State> CreateStateList(bool throw_if_unknown=true) const {
		std::vector<State> states;
		states.reserve(items_.size());
		for(ConstIt it=items_.begin(); it!=items_.end(); ++it) {
			if(it->is_known_) {
				states.push_back(it->state_);
			} else {
				if(throw_if_unknown) {
					throw StateNotKnownException();
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
			if(i.is_evaluated_) {
				os << i.index_ << "\tTime=" << i.time_ << "\tScore=" << i.score_ << "\tState=" << i.state_;
			}
			else if(i.is_known_) {
				os << i.index_ << "\tTime=" << i.time_ << "\tScore=unknown\tState=" << i.state_;
			}
			else {
				os << i.index_ << "\tTime=" << i.time_ << "\tScore=unknown\tState=unknown";
			}
		}
		os << "\n]";
	}

private:
	void Initialize(size_t count) {
		items_.resize(count);
		size_t i = 0;
		for(It it=items_.begin(); it!=items_.end(); ++it, i++) {
			it->index_ = i;
			it->is_known_ = false;
			it->is_evaluated_ = false;
		}
	}

	template<typename I2T>
	void SetTimes(const I2T& op) {
		for(It it=items_.begin(); it!=items_.end(); ++it) {
			it->time_ = op(it->index_);
		}
	}

private:
	std::vector<Item> items_;
};

//---------------------------------------------------------------------------

template<typename Time, typename State, typename Score>
struct TRange
{
	TSolution<Time, State, Score> solution_;
	size_t begin_, end_;
	size_t strive_;
};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
