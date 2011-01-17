/*
 * Functions.h
 *
 *  Created on: Sep 15, 2010
 *     Changed: 17.01.2011
 *      Author: david
 */

#ifndef Q0_TRACKING_FUNCTIONS_H_
#define Q0_TRACKING_FUNCTIONS_H_
//---------------------------------------------------------------------------
#include <vector>
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

template<typename Time, typename State, typename Score>
struct SeparateDelegateFunction
{
	typedef boost::function<std::vector<Score>(const std::vector<State>& states)> EvaluateFunctorType;
	typedef boost::function<void(Time)> SetTimeFunctorType;

	SeparateDelegateFunction() : has_time_(false) {}

	void set_functor_evaluate(const EvaluateFunctorType& f) { func_evaluate_ = f; }

	void set_functor_set_time(const SetTimeFunctorType& f) { func_set_time_ = f; }

	Score operator()(Time t, const State& state) const {
		SetTime(t);
		// FIXME use special function!
		std::vector<State> states;
		states.push_back(state);
		return func_evaluate_(states)[0];
	}

	std::vector<Score> operator()(Time t, const std::vector<State>& states) const {
		SetTime(t);
		return func_evaluate_(states);
	}

private:
	void SetTime(Time t) const {
		if(!has_time_ || t != time_) {
			has_time_ = true;
			time_ = t;
			func_set_time_(t);
		}
	}

private:
	mutable bool has_time_;
	mutable Time time_;
	EvaluateFunctorType func_evaluate_;
	SetTimeFunctorType func_set_time_;
};

//---------------------------------------------------------------------------

template<typename Time, typename State, typename Score, class VarFunction>
struct PinnedFunction
{
	PinnedFunction(const VarFunction& f)
	: f_(f) {}

	PinnedFunction(const VarFunction& f, const Time& t)
	: f_(f), time_(t) {}

	void setTime(const Time& t) { time_ = t; }

	Score operator()(const State& state) const {
		return f_(time_, state);
	}

	std::vector<Score> operator()(const std::vector<State>& states) const {
		return f_(time_, states);
	}

private:
	VarFunction f_;
	Time time_;
};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
