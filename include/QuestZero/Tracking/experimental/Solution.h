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
#include <ostream>
#include <vector>
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

template<typename Time, typename State, typename Score>
struct TSolution
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

	TSolution(const std::vector<Time>& times) {
		this->Initialize(times.size());
		this->SetTimes(times);
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

	std::vector<Time> GetTimes() const {
		std::vector<Time> vt;
		vt.reserve(items_.size());
		for(ConstIt it=items_.begin(); it!=items_.end(); ++it) {
			vt.push_back(it->time_);
		}
		return vt;
	}

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

	/** Gets the index of the previous known time tick */
	size_t SearchPreviousKnown(size_t start) const {
		for(ConstIt it=items_.begin()+start; it>=items_.begin(); --it) {
			if(it->is_known_) {
				return it->index_;
			}
		}
		return static_cast<size_t>(-1);
	}

	size_t SearchNearestKnown(size_t start) const {
		assert(start < items_.size());
		size_t p1 = SearchPreviousKnown(start);
		bool p1_ok = (p1 != static_cast<size_t>(-1));
		size_t p2 = SearchNextKnown(start);
		bool p2_ok = (p2 != items_.size());
		if(p1_ok && p2_ok) {
			assert(p1 <= start);
			assert(start <= p2);
			if(start - p1 <= p2 - start) {
				return p1;
			}
			else {
				return p2;
			}
		}
		else if(p1_ok) {
			return p1;
		}
		else if(p2_ok) {
			return p2;
		} else {
			return items_.size(); // can not use any!
		}
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

	void Set(const TSolution& s, size_t pos=0) {
		assert(pos + s.items_.size() <= items_.size());
		for(unsigned int i=0; i<s.items_.size(); i++) {
			items_[pos + i] = s.items_[i];
		}
	}

	void Set(size_t index, Time time) {
		Item& a = items_[index];
		a.time_ = time;
		//a.state_ = state;
		a.is_known_ = false;
		a.score_ = Score(0);
		a.is_evaluated_ = false;
	}

	void Set(size_t index, const State& state) {
		Item& a = items_[index];
		a.state_ = state;
		a.is_known_ = true;
		a.score_ = Score(0);
		a.is_evaluated_ = false;
	}

	void SetScore(size_t index, Score score) {
		Item& a = items_[index];
		a.score_ = score;
		a.is_evaluated_ = true;
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

	Time GetFirstTime() const {
		return items_.front().time_;
	}

	Time GetLastTime() const {
		return items_.back().time_;
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
			if(!it->is_known_) {
				return false;
			}
		}
		return true;
	}

	bool AreAllEvaluated() const {
		for(ConstIt it=items_.begin(); it!=items_.end(); ++it) {
			if(!it->is_evaluated_) {
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
			if(it->is_evaluated_) {
				scores.push_back(it->score_);
			}
			else if(it->is_known_) {
				scores.push_back(Score(0)); // so that we have the number of elements as CreateStateList!
			}
			else {
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
			}
			else {
				if(throw_if_unknown) {
					throw StateNotKnownException();
				}
			}
		}
		return states;
	}

	void print(std::ostream& os) const {
		os << "Solution=[";
		size_t unkown_count = 0;
		bool write_newline = true;
		for(ConstIt it=items_.begin(); it!=items_.end(); ++it) {
			const Item& i = *it;
			if(write_newline) {
				os << "\n";
			}
			write_newline = true;
			if(i.is_evaluated_) {
				os << i.index_ << "\tTime=" << i.time_ << "\tScore=" << i.score_ << "\tState=" << i.state_;
				unkown_count = 0;
			}
			else if(i.is_known_) {
				os << i.index_ << "\tTime=" << i.time_ << "\tScore=unknown\tState=" << i.state_;
				unkown_count = 0;
			}
			else {
				if(unkown_count < 3 || it + 1 == items_.end()) {
					os << i.index_ << "\tTime=" << i.time_ << "\tScore=unknown\tState=unknown";
				}
				else if(unkown_count == 3) {
					os << "\t...";
				}
				else {
					write_newline = false;
				}
				unkown_count++;
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

	void SetTimes(const std::vector<Time>& times) {
		for(It it=items_.begin(); it!=items_.end(); ++it) {
			it->time_ = times[it->index_];
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

template<typename Time, typename State, typename Score>
std::ostream& operator<<(std::ostream& os, const TSolution<Time, State, Score>& solution) {
	solution.print(os);
	return os;
}

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
