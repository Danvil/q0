/*
 * RState.h
 *
 *  Created on: Sep 5, 2010
 *      Author: david
 */

#ifndef QUESTZERO_RSTATE_H_
#define QUESTZERO_RSTATE_H_

#include <Danvil/Print.h>
#include <Danvil/Ptr.h>
#include <Danvil/LinAlg.h>
#include <Danvil/LinAlg/RotationTools.h>
#include <Danvil/Tools/Small.h>
#include <Danvil/Memops/Copy.h>
#include <vector>
#include <boost/foreach.hpp>
#include <boost/range.hpp>
#include <stdexcept>

template<typename K>
class TRState
: public Danvil::Print::IPrintable
{
public:
	typedef Danvil::ctLinAlg::TQuaternion<K> Q;

public:
	TRState() {}

	TRState(const Q& q)
	: _rot(q) {}

	TRState(const TRState& s)
	: _rot(s._rot) {}

	TRState& operator=(const TRState& s) {
		_rot = s._rot;
		return *this;
	}

	const Q& rot() const { return _rot; }

	unsigned int dimension() const {
		return 3;
	}

	size_t numbersCount() const {
		return 4;
	}

	void toNumbers(size_t n, K* data) const {
		assert(n == numbersCount());
		data[0] = _rot.w;
		data[1] = _rot.x;
		data[2] = _rot.y;
		data[3] = _rot.z;
	}

	void fromNumbers(size_t n, const K* data) {
		assert(n == numbersCount());
		_rot.w = data[0];
		_rot.x = data[1];
		_rot.y = data[2];
		_rot.z = data[3];
	}

	void print(std::ostream& os) const {
		os << _rot;
	}

private:
	Q _rot;

};

template<typename K>
class TRStateOperator
{
public:
	typedef Danvil::ctLinAlg::TQuaternion<K> Q;
	typedef TRState<K> State;

public:
	static K Distance(const State& a, const State& b) {
		return Q::Distance(a, b);
	}

	static State Compose(const State& a, const State& b) {
		return State(a.rot() * b.rot());
	}

	static State Difference(const State& a, const State& b) {
		return State(a.rot() * b.rot().inverse());
	}

	static State WeightedSum(K f1, const State& s1, K f2, const State& s2, K f3, const State& s3) {
		std::vector<K> factors;
		factors.push_back(f1);
		factors.push_back(f2);
		factors.push_back(f3);
		std::vector<State> states;
		states.push_back(s1);
		states.push_back(s2);
		states.push_back(s3);
		return WeightedSum(factors, states);
	}

	static State WeightedSum(size_t n, const K* factors, const State* states) {
		if(n == 0) {
			throw std::runtime_error("Must have at least one element for WeightedSum!");
		}
		return WeightesSum(std::vector<K>(n, factors), std::vector<State>(n, states));
	}

	static State WeightedSum(const std::vector<K>& factors, const std::vector<State>& states) {
		if(factors.size() != states.size()) {
			throw std::runtime_error("Number of factors and states must be equal!");
		}
		std::vector<Q> qs;
		for(size_t i=0; i<states.size(); i++) {
			qs.push_back(states[i].rot());
		}
		Q mean = Danvil::ctLinAlg::RotationTools::WeightedMean(qs, factors, 1e-3);
		return State(mean);
	}

	static State Mean(const std::vector<State>& states) {
		return WeightedSum(std::vector<K>(states.size(), (K)1), states);
	}

};

#include "../RandomNumbers.h"

template<typename K>
class TRFull
{
public:
	typedef Danvil::ctLinAlg::TQuaternion<K> Q;
	typedef TRState<K> State;

public:
	TRFull() {}

	State random() const {
		return State(RandomQ());
	}

	State random(const State& center, K noise) {
		return State(center.rot() * RandomQ(noise));
	}

	std::vector<State> random(size_t n) const {
		std::vector<State> states;
		for(size_t i=0; i<n; i++) {
			states.push_back(random());
		}
		return states;
	}

	State project(const State& s) const {
		return State(ProjectQ(s.rot()));
	}

private:
	static Q RandomQ() {
		return Danvil::ctLinAlg::RotationTools::UniformRandom<K>(&RandomNumbers::Random01);
	}

	static Q RandomQ(K len) {
		return Danvil::ctLinAlg::RotationTools::UniformRandom<K>(len, &RandomNumbers::Random01);
	}

	static Q ProjectQ(const Q& x) {
		Q q = x;
		q.normalize();
		return q;
	}

};

#endif
