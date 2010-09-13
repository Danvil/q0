/*
 * Multiplier.h
 *
 *  Created on: Sep 11, 2010
 *      Author: david
 */

#ifndef MULTIPLIER_H_
#define MULTIPLIER_H_

#include "GetScalarType.h"
#include <Danvil/Print.h>
#include <vector>

namespace Spaces
{

template<typename BaseState, unsigned int _N>
struct MultiplierState
: public Danvil::Print::IPrintable
{
	typedef typename Private::GetScalarType<BaseState>::ScalarType ScalarType;

	static const unsigned int N = _N;

	BaseState& operator[](int i) {
		return sub[i];
	}

	const BaseState& operator[](int i) const {
		return sub[i];
	}

	void print(std::ostream& os) const {
		os << "[";
		for(unsigned int i=0; i<N; i++) {
			os << i << "=" << sub[i];
			if(i != N-1) {
				os << ", ";
			}
		}
		os << "]";
	}

private:
	BaseState sub[N];

};

template<typename BaseSpace, typename State_>
struct MultiplierSpace
{
	typedef State_ State;

	static const unsigned int N = State::N;

	struct InvalidNoiseVectorException {};

	struct WeightedSumException {};

	BaseSpace& operator[](int i) {
		return spaces[i];
	}

	const BaseSpace& operator[](int i) const {
		return spaces[i];
	}

	unsigned int dimension() const {
		unsigned int n = 0;
		for(unsigned int i=0; i<N; i++) {
			n += spaces[i].dimension();
		}
		return n;
	}

	double distance(const State& a, const State& b) const {
		double d = 0;
		for(unsigned int i=0; i<N; ++i) {
			d += spaces[i].distance(a[i], b[i]);
		}
		return d;
	}

	State inverse(const State& a) const {
		State s;
		for(unsigned int i=0; i<N; ++i) {
			s[i] = spaces[i].inverse(a[i]);
		}
		return s;
	}

	State compose(const State& a, const State& b) const {
		State s;
		for(unsigned int i=0; i<N; ++i) {
			s[i] = spaces[i].compose(a[i], b[i]);
		}
		return s;
	}

	template<typename K>
	State weightedSum(K f1, const State& s1, K f2, const State& s2) const {
		State s;
		for(unsigned int i=0; i<N; ++i) {
			s[i] = spaces[i].weightedSum(f1, s1[i], f2, s2[i]);
		}
		return s;
	}

	template<typename K>
	State weightedSum(K f1, const State& s1, K f2, const State& s2, K f3, const State& s3) const {
		State s;
		for(unsigned int i=0; i<N; ++i) {
			s[i] = spaces[i].weightedSum(f1, s1[i], f2, s2[i], f3, s3[i]);
		}
		return s;
	}

	template<typename K>
	State weightedSum(const std::vector<K>& factors, const std::vector<State>& states) const {
		if(factors.size() != states.size()) {
			// Number of factors and states must be equal!
			throw WeightedSumException();
		}
		if(states.size() == 0) {
			// Must have at least one element for WeightedSum!
			throw WeightedSumException();
		}
		State s;
		for(unsigned int i=0; i<N; ++i) {
			std::vector<State> parts;
			parts.reserve(states.size());
			for(size_t k=0; k<states.size(); k++) {
				parts[k] = states[k][i];
			}
			s[i] = weightedSum(factors, states);
		}
		return s;
	}

	State project(const State& s) const {
		State v;
		for(size_t i=0; i<N; i++) {
			v[i] = spaces[i].project(s[i]);
		}
		return v;
	}

	State random() const {
		State v;
		for(size_t i=0; i<N; i++) {
			v[i] = spaces[i].random();
		}
		return v;
	}

	template<typename K>
	State random(const State& center, const std::vector<K>& noise) const {
		State v;
		size_t start = 0;
		for(size_t i=0; i<N; i++) {
			size_t len = spaces[i].dimension();
			if(start + len > noise.size()) {
				throw InvalidNoiseVectorException();
			}
			v[i] = spaces[i].random(center[i], std::vector<K>(noise.begin() + start, noise.begin() + start + len));
			start += len;
		}
		return v;
	}

	// TODO this is default
	State difference(const State& a, const State& b) const {
		return compose(a, inverse(b));
	}

	// TODO this is default
	State mean(const std::vector<State>& states) const {
		typedef typename State::ScalarType Scalar;
		return weightedSum(std::vector<Scalar>(states.size(), (Scalar)1), states);
	}

	// TODO this is default
	std::vector<State> randomMany(size_t n) const {
		std::vector<State> states;
		for(size_t i=0; i<n; i++) {
			states.push_back(this->random());
		}
		return states;
	}

private:
	BaseSpace spaces[N];

};

}

#endif
