/*
 * TargetPolicy.h
 *
 *  Created on: Sep 12, 2010
 *      Author: david
 */

#ifndef POLICIES_TARGETPOLICY_H_
#define POLICIES_TARGETPOLICY_H_

namespace TargetPolicy
{
	template<typename ScoreType=double, typename CounterType=unsigned long>
	struct FixedChecks
	{
		FixedChecks(const CounterType& max=10)
		: _current(0),
		  _max(max)
		{}

		void setMaxCount(const CounterType& max) {
			_max = max;
		}

		const CounterType& maxCount() const { return _max; }

		const CounterType& currentCount() const { return _current; }

		bool reached(const ScoreType&) {
			_current ++;
			return _current >= _max;
		}

	private:
		CounterType _current;
		CounterType _max;
	};

	/** Attention: This may result in an infinite loop. Better use ScoreTargetWithMaxChecks! */
	template<typename ScoreType=double>
	struct ScoreTarget
	{
		ScoreTarget(const ScoreType& t=1e-3)
		: _target(t)
		{}

		void setTargetScore(const ScoreType& t) {
			_target = t;
		}

		const ScoreType& targetScore() const {
			return _target;
		}

		bool reached(const ScoreType& score) const {
			return score <= _target;
		}

	private:
		ScoreType _target;
	};

	template<typename ScoreType=double, typename CounterType=unsigned long>
	struct ScoreTargetWithMaxChecks
	: public FixedChecks<ScoreType,CounterType>,
	  public ScoreTarget<ScoreType>
	{
		bool reached(const ScoreType& score) {
			return FixedChecks<ScoreType,CounterType>::reached(score) // must be first, so it gets evaluated every time!
				|| ScoreTarget<ScoreType>::reached(score);
		}

	};

}

#endif
