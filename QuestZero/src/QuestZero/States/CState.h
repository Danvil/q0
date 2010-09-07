/*
 * CState.h
 *
 *  Created on: Sep 5, 2010
 *      Author: david
 */

#ifndef QUESTZERO_CSTATE_H_
#define QUESTZERO_CSTATE_H_

#include <Danvil/Print.h>
#include <Danvil/Ptr.h>
#include <Danvil/LinAlg.h>
#include <Danvil/Tools/Small.h>
#include <Danvil/Memops/Copy.h>
#include <vector>
#include <boost/foreach.hpp>
#include <boost/range.hpp>
#include <stdexcept>

template<typename K, int N_CAT>
class TCState
: public Danvil::Print::IPrintable
{
public:
	typedef Danvil::ctLinAlg::Vec<K, N_CAT> V;

public:
	V cartesian;

public:
	TCState() {}

	TCState(const V& v)
	: cartesian(v) {}

	unsigned int dimension() const {
		return N_CAT;
	}

	size_t numbersCount() const {
		return N_CAT;
	}

	void toNumbers(size_t n, K* data) const {
		assert(n == numbersCount());
		Danvil::Memops::Copy<K, N_CAT>(data, cartesian.begin());
	}

	void fromNumbers(size_t n, const K* data) {
		assert(n == numbersCount());
		Danvil::Memops::Copy<K, N_CAT>(cartesian.begin(), data);
	}

	void print(std::ostream& os) const {
		os << cartesian;
	}

};

template<typename K, int N_CAT>
class TCStateOperator
{
public:
	typedef TCState<K, N_CAT> State;

public:
	static K Distance(const State& a, const State& b) {
		return Danvil::ctLinAlg::Distance(a, b);
	}

	static State Compose(const State& a, const State& b) {
		return State(a.cartesian + b.cartesian);
	}

	static State Difference(const State& a, const State& b) {
		return State(a.cartesian - b.cartesian);
	}

	static State WeightedSum(K f1, const State& s1, K f2, const State& s2, K f3, const State& s3) {
		return State(f1 * s1.cartesian + f2 * s2.cartesian + f3 * s3.cartesian);
	}

	static State WeightedSum(size_t n, const K* factors, const State* states) {
		if(n == 0) {
			throw std::runtime_error("Must have at least one element for WeightedSum!");
		}
		State c(factors[0] * states[0].cartesian);
		for(int i=1; i<n; i++) {
			c.cartesian += factors[i] * states[i].cartesian;
		}
		return c;
	}

	static State WeightedSum(const std::vector<K>& factors, const std::vector<State>& states) {
		if(factors.size() != states.size()) {
			throw std::runtime_error("Number of factors and states must be equal!");
		}
		return WeightedSum(factors.size(), factors.base(), states.base());
	}

};

#include "../RandomNumbers.h"

template<typename K, int N_CAT>
class TCDomainBox
{
public:
	typedef Danvil::ctLinAlg::Vec<K, N_CAT> V;
	typedef TCState<K, N_CAT> State;

public:
	TCDomainBox(const V& min, const V& max)
	: _min(min),
	  _max(max) {}

	State random() const {
		return State(randomV());
	}

	State random(const State& center, const std::vector<K>& noise) {
		if(noise.size() != N_CAT) {
			throw std::runtime_error("Noise vector has invalid length!");
		}
		V noise_range(noise.data());
		return State(projectV(center.cartesian + RandomV(-noise_range, noise_range)));
	}

	std::vector<State> random(size_t n) const {
		std::vector<State> states;
		for(size_t i=0; i<n; i++) {
			states.push_back(random());
		}
		return states;
	}

	State project(const State& s) const {
		return State(projectV(s.cartesian));
	}

private:
	V randomV() const {
		return RandomV(_min, _max);
	}

	static V RandomV(const V& min, const V& max) {
		V v;
		for(size_t i=0; i<v.dimension(); i++) {
			v[i] = RandomNumbers::S.random(min[i], max[i]);
		}
		return v;
	}

	V projectV(const V& x) const {
		V v;
		for(size_t i=0; i<v.dimension(); i++) {
			v[i] = Danvil::clamped(x[i], _min[i], _max[i]);
		}
		return v;
	}

private:
	V _min, _max;

};


#endif
