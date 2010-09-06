#ifndef QUEST_ZERO_IFUNCTION_H
#define QUEST_ZERO_IFUNCTION_H

#include <vector>

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

#endif

