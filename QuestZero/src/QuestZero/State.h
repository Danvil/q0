/*
 * State.h
 *
 *  Created on: Sep 4, 2010
 *      Author: david
 */

#ifndef STATE_H_
#define STATE_H_

#include <Danvil/Ptr.h>
#include <Danvil/LinAlg.h>
#include <vector>
#include <boost/foreach.hpp>
#include <boost/range.hpp>
#include <stdexcept>

template<typename K, int N_CAT, int N_ROT>
class State
{
public:
	Danvil::ctLinAlg::Vec<K, N_CAT> cartesian;
	Danvil::ctLinAlg::Quaternion<K> rotation[N_ROT];

public:
	unsigned int dimension() const {
		return N_CAT + 3*N_ROT;
	}

	void toNumbers(int n, K* data) const {
		throw std::runtime_error("Not implemented!");
	}

	void fromNumbers(int n, K* data) {
		throw std::runtime_error("Not implemented!");
	}

	static K Distance(const State& a, const State& b) {
		K d = (K)0;
		for(int i=0; i<N_CAT; i++) {
			K x = a.cartesian[i] - b.cartesian[i];
			d += x * x;
		}
		for(int i=0; i<N_ROT; i++) {
			d += shortestAngle(a.rotation[i], b.rotation[i]);
		}
	}

	static State& Compose(const State& a, const State& b) {
		State c;
		c.cartesian = a.cartesian + b.cartesian;
		for(int i=0; i<N_ROT; i++) {
			c.rotation[i] = a.rotation[i] * b.rotation[i];
		}
		return c;
	}

	static State& Difference(const State& a, const State& b) {
		State c;
		c.cartesian = a.cartesian - b.cartesian;
		for(int i=0; i<N_ROT; i++) {
			c.rotation[i] = b.rotation[i].inverse() * a.rotation[i];
		}
		return c;
	}

	static State& WeightedSum(double factors[], State states[]) {
		throw std::runtime_error("Not implemented!");
	}

	static std::vector<State> Random(size_t n) {
		throw std::runtime_error("Not implemented!");
	}

};

#endif
