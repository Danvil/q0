#ifndef QUEST_ZERO_IFUNCTION_H
#define QUEST_ZERO_IFUNCTION_H

#include <vector>
#include <boost/function.hpp>

template<typename State>
class IFunction
{
public:
	virtual double operator()(const State& state) = 0;

	virtual std::vector<double> evaluateParallel(const std::vector<State>& states) {
		std::vector<double> r;
		r.reserve(states.size());
		for(size_t i=0; i<states.size(); i++) {
			r.push_back((*this)(states[i]));
		}
		return r;
	}

};

template<typename State>
struct FunctionFromDelegate
{
	typedef boost::function<double(const State&)> FunctionType;

	FunctionFromDelegate(const FunctionType& f)
	: _function(f) {}

	double evaluate(const State& state) {
		return _function(state);
	}

	std::vector<double> evaluateParallel(const std::vector<State>& states) {
		std::vector<double> scores;
		scores.reserve(states.size());
		for(size_t i=0; i<states.size(); ++i) {
			scores.push_back(evaluate(states[i]));
		}
		return scores;
	}

private:
	FunctionType _function;
};

template<typename State>
struct FunctionFromParallelDelegate
{
	typedef boost::function<std::vector<double>(const std::vector<State>&)> FunctionType;

	FunctionFromParallelDelegate(const FunctionType& f)
	: _function(f) {}

	double evaluate(const State& state) {
		return evaluateParallel(std::vector<State>(1, state))[0];
	}

	std::vector<double> evaluateParallel(const std::vector<State>& states) {
		return _function(states);
	}

private:
	FunctionType _function;
};

#endif

