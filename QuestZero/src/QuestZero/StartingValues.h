/*
 * StartingValues.h
 *
 *  Created on: Sep 7, 2010
 *      Author: david
 */

#ifndef STARTINGVALUES_H_
#define STARTINGVALUES_H_

#include <Danvil/Ptr.h>
#include <vector>

class StartingValues
{
public:
	template<typename State, typename Domain>
	static std::vector<State> Random(const PTR(Domain)& dom, unsigned int n) {
		std::vector<State> states;
		states.reserve(n);
		for(unsigned int i=0; i<n; i++) {
			states.push_back(dom.random());
		}
		return states;
	}

	template<typename State>
	static std::vector<State> Repeat(const State& state, unsigned int n) {
		return std::vector<State>(n, state);
	}

	template<typename State>
	static std::vector<State> Repeat(const std::vector<State>& states, unsigned int n) {
		if(states.size() == n) {
			return states;
		} else if(states.size() > n) {
			return std::vector<State>(states.begin(), states.begin() + n);
		} else {
			std::vector<State> result;
			result.reserve(n);
			for(unsigned int i=0; i<n; i++) {
				result.push_back(states[i % states.size()]);
			}
			return result;
		}
	}

	template<typename State, typename Domain>
	static std::vector<State> WithNoise(const State& state, unsigned int n, const PTR(Domain)& dom, const std::vector<typename State::ScalarType>& noise) {
		std::vector<State> states;
		states.reserve(n);
		for(unsigned int i=0; i<n; i++) {
			State state_with_noise = dom->random(state, noise);
			states.push_back(state_with_noise);
		}
		return states;
	}

private:
	StartingValues() {}

};

#endif
