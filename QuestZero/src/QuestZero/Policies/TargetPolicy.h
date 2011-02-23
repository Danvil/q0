/*
 * TargetPolicy.h
 *
 *  Created on: Sep 12, 2010
 *      Author: david
 */

#ifndef POLICIES_TARGETPOLICY_H_
#define POLICIES_TARGETPOLICY_H_
//---------------------------------------------------------------------------
#include <iostream>
#include <cfloat>
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

namespace TargetPolicy
{
	/** Breaks after a given number of iterations */
	template<typename ScoreType=double, bool Console=true>
	struct FixedChecks
	{
		FixedChecks(size_t max=10)
		: current_(0),
		  max_(max)
		{}

		void SetMaximumIterationCount(size_t max) { max_ = max; }

		size_t GetMaximumIterationCount() const { return max_; }

		size_t GetCurrentIterationCount() const { return current_; }

		bool IsTargetReached(ScoreType) {
			++current_;
			if(Console) {
				std::cout << "Iteration " << current_ << "/" << max_ << std::endl;
			}
			return current_ >= max_;
		}

	private:
		size_t current_;
		size_t max_;
	};

	/** Breaks when the score is smaller than a given value
	 * Attention: This may result in an infinite loop. Better use ScoreTargetWithMaxChecks!
	 */
	template<typename ScoreType=double, bool Console=true>
	struct ScoreTarget
	{
		ScoreTarget(ScoreType t=1e-3)
		: current_(FLT_MAX), goal_(t)
		{}

		void SetTargetScore(ScoreType t) { goal_ = t; }

		ScoreType GetTargetScore() const { return goal_; }

		ScoreType GetCurrentScore() const { return current_; }

		bool IsTargetReached(ScoreType score) {
			current_ = score;
			if(Console) {
				std::cout << "Current Score=" << score << " (Target=" << goal_ << ")" << std::endl;
			}
			return current_ <= goal_;
		}

	private:
		ScoreType current_;
		ScoreType goal_;
	};

	/** Breaks if a score goal has been reached or after a maximum number of iterations */
	template<typename ScoreType=double, bool Console=true>
	struct ScoreTargetWithMaxChecks
	: public FixedChecks<ScoreType,false>,
	  public ScoreTarget<ScoreType,false>
	{
		bool IsTargetReached(const ScoreType& score) {
			bool check_iters = ((FixedChecks<ScoreType,false>*)this)->IsTargetReached(score); // must be first, so it gets evaluated every time!
			bool check_score = ((ScoreTarget<ScoreType,false>*)this)->IsTargetReached(score);
			if(Console) {
				std::cout << "Iteration " << this->GetCurrentIterationCount() << "/" << this->GetMaximumIterationCount() << ": "
						<< "Current Score=" << this->GetCurrentScore() << " (Target=" << this->GetTargetScore() << ")" << std::endl;
			}
			return check_iters || check_score;
		}

	};

}

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
