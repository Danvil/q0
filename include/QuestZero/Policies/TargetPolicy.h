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
	template<typename Traits, typename A, typename B>
	struct FullThingy {};

	template<typename A, typename B>
	struct FullThingy_Resolver {
		template<typename Traits>
		struct resolver {
			typedef FullThingy<Traits,A,B> result_type;
		};
	};

	FullThingy_Resolver<int,int>::resolver<int>::result_type X;

	/** Breaks after a given number of iterations */
	template<typename ScoreType, bool Console=true>
	struct FixedChecks
	{
		FixedChecks(size_t max=10)
		: current_(0),
		  max_(max)
		{}

		void SetMaximumIterationCount(size_t max) { max_ = max; }

		size_t GetMaximumIterationCount() const { return max_; }

		size_t GetCurrentIterationCount() const { return current_; }

		template<typename Compare>
		bool IsTargetReached(ScoreType, Compare) {
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
	template<typename ScoreType, bool Console=true>
	struct ScoreTarget
	{
		ScoreTarget(ScoreType t=1e-3)
		: current_(FLT_MAX), goal_(t)
		{}

		void SetTargetScore(ScoreType t) { goal_ = t; }

		ScoreType GetTargetScore() const { return goal_; }

		ScoreType GetCurrentScore() const { return current_; }

		template<typename Compare>
		bool IsTargetReached(ScoreType score, Compare cmp) {
			current_ = score;
			if(Console) {
				std::cout << "Current Score=" << score << " (Target=" << goal_ << ")" << std::endl;
			}
			return cmp(current_, goal_);
		}

		// FIXME this is specific for Nelder-Mead - remove it
		template<typename Compare>
		bool IsTargetReached(ScoreType score, ScoreType test, Compare cmp) {
			current_ = score;
			if(Console) {
				std::cout << "Current Score=" << score << ", Test=" << test << " (Target=" << goal_ << ")" << std::endl;
			}
			return cmp(test, goal_);
		}

	private:
		ScoreType current_;
		ScoreType goal_;
	};

	/** Breaks if a score goal has been reached or after a maximum number of iterations */
	template<typename ScoreType, bool Console=true>
	struct ScoreTargetWithMaxChecks
	: public FixedChecks<ScoreType,false>,
	  public ScoreTarget<ScoreType,false>
	{
		template<typename Compare>
		bool IsTargetReached(ScoreType score, Compare cmp) {
			bool check_iters = ((FixedChecks<ScoreType,false>*)this)->IsTargetReached(score, cmp); // must be first, so it gets evaluated every time!
			bool check_score = ((ScoreTarget<ScoreType,false>*)this)->IsTargetReached(score, cmp);
			if(Console) {
				std::cout << "Iteration " << this->GetCurrentIterationCount() << "/" << this->GetMaximumIterationCount() << ": "
						<< "Current Score=" << this->GetCurrentScore() << " (Target=" << this->GetTargetScore() << ")" << std::endl;
			}
			return check_iters || check_score;
		}

		// FIXME this is specific for Nelder-Mead - remove it
		template<typename Compare>
		bool IsTargetReached(ScoreType score, ScoreType test, Compare cmp) {
			bool check_iters = ((FixedChecks<ScoreType,false>*)this)->IsTargetReached(score, cmp); // must be first, so it gets evaluated every time!
			bool check_score = ((ScoreTarget<ScoreType,false>*)this)->IsTargetReached(score, test, cmp);
			if(Console) {
				std::cout << "Iteration " << this->GetCurrentIterationCount() << "/" << this->GetMaximumIterationCount() << ": "
						<< "Current Score=" << this->GetCurrentScore() << ", Test=" << test << " (Target=" << this->GetTargetScore() << ")" << std::endl;
			}
			return check_iters || check_score;
		}

	};

	template<typename Score, bool Console=true>
	struct ScoreChangeTarget
	{
		typedef std::function<double(Score,Score)> DeltaFunctor;

		ScoreChangeTarget()
		: has_last_score_(false),
		  target_delta_(0.0),
		  fnc_delta_([](Score x, Score y) { return 0.0; }) // FIXME is it good to define a default?
		{}

		void SetTargetDelta(double delta) {
			target_delta_ = delta;
		}

		void SetTargetDeltaFunctor(DeltaFunctor fnc) {
			fnc_delta_ = fnc;
		}

		template<typename Compare>
		bool IsTargetReached(Score score, Compare cmp) {
			if(!has_last_score_) {
				has_last_score_ = true;
				last_score_ = score;
				return false;
			}
			double d = fnc_delta_(score, last_score_);
			if(Console) {
				std::cout << "Current score=" << score << ", Last score=" << last_score_ << ", Delta=" << d << ", Target delta=" << target_delta_ << std::endl;
			}
			last_score_ = score;
			if(d <= target_delta_) {
				return true;
			}
			else {
				return false;
			}
		}

	private:
		bool has_last_score_;
		Score last_score_;
		double target_delta_;
		DeltaFunctor fnc_delta_;
	};

	/** Checks abs(score - last_score) <= target */
	template<typename Score, bool Console>
	void set_absdiff_score_change_target(ScoreChangeTarget<Score,Console>& obj, double target) {
		obj.SetTargetDelta(target);
		obj.SetTargetDeltaFunctor([](Score x, Score y) { return std::abs(x - y); });
	}


}

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
