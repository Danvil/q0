/*
 * PinnedFunction.h
 *
 *  Created on: Sep 15, 2010
 *      Author: david
 */

#ifndef PINNEDFUNCTION_H_
#define PINNEDFUNCTION_H_

#include <vector>

template<typename Time, typename State, class VarFunction>
struct PinnedFunction
{
	PinnedFunction(const VarFunction& f, const Time& t)
	: f_(f),
	  time_(t)
	{}

	void setTime(const Time& t) { time_ = t; }

	double operator()(const State& state) const {
		return f_(time_, state);
	}

	std::vector<double> operator()(const std::vector<State>& states) const {
		return f_(time_, states);
	}

private:
	VarFunction f_;
	Time time_;
};

#endif
