/*
 * TargetPolicy.h
 *
 *  Created on: Sep 12, 2010
 *      Author: david
 */

#ifndef POLICIES_TARGETPOLICY_H_
#define POLICIES_TARGETPOLICY_H_
//---------------------------------------------------------------------------
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <iostream>
#include <cfloat>
#include <cmath>
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

namespace ExitPolicy
{
	/** Exits (the latest) after a given number of iterations */
	template<typename ScoreType, bool Console=false>
	struct FixedChecks
	{
		FixedChecks(size_t max=10)
		: exit_current_iteration_(0),
		  exit_num_iterations_(max)
		{}

		/** Sets the (maximal) number of iterations the algorithm should run */
		void SetIterationCount(size_t num_iterations) {
			exit_num_iterations_ = num_iterations;
		}

		/** Gets the (maximum) number of iterations the algorithm will run */
		size_t GetIterationCount() const {
			return exit_num_iterations_;
		}

		/** Gets the current iteration the algorithm is in */
		size_t GetCurrentIteration() const {
			return exit_current_iteration_;
		}

		/** Checks if the exit condition is met */
		template<typename Compare>
		bool IsTargetReached(ScoreType score, Compare) {
			++exit_current_iteration_;
			if(Console) {
				std::cout << "Iteration " << exit_current_iteration_ << "/" << exit_num_iterations_ << ": Current Score=" << score << std::endl;
			}
			return exit_current_iteration_ >= exit_num_iterations_;
		}

	private:
		size_t exit_current_iteration_;
		size_t exit_num_iterations_;
	};

	/** Exits when the score is smaller than a given value
	 * Attention: This may result in an infinite loop. Better use ScoreTargetWithMaxChecks!
	 */
	template<typename ScoreType, bool Console=false>
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

	private:
		ScoreType current_;
		ScoreType goal_;
	};

	/** Breaks if a score goal has been reached or after a maximum number of iterations */
	template<typename ScoreType, bool Console=false>
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

	};

	template<typename Score, bool Console=false>
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
	void set_score_change_exit_policy(ScoreChangeTarget<Score,Console>& obj, double target) {
		obj.SetTargetDelta(target);
		obj.SetTargetDeltaFunctor([](Score x, Score y) { return std::abs(x - y); });
	}

	template<typename Score, bool Console=false>
	struct StdDevExitPolicy
	{
		template<typename SampleSet>
		bool IsTargetReached(const SampleSet& sample_list) {
			using namespace boost::accumulators;
			accumulator_set<double, stats<tag::variance>> score_acc;
			for(auto id : samples(sample_list)) {
				score_acc(get_score(sample_list, id));
			}
			double stddev = std::sqrt(variance(score_acc));
			if(Console) {
				std::cout << "Current std. dev. =" << stddev << ", Target =" << target_ << std::endl;
			}
			return stddev < target_;
		}

		void SetExitStdDevTarget(double target) {
			target_ = target;
		}

	private:
		double target_;
	};

}

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
