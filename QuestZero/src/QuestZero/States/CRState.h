/*
 * CRState.h
 *
 *  Created on: Sep 5, 2010
 *      Author: david
 */

#ifndef QUESTZERO_CRSTATE_H_
#define QUESTZERO_CRSTATE_H_

#include "CState.h"
#include "RState.h"
#include <Danvil/Ptr.h>
#include <Danvil/LinAlg.h>
#include <vector>
#include <boost/foreach.hpp>
#include <boost/range.hpp>
#include <stdexcept>

template<typename K, int N_CAT, int N_ROT>
class TCRState
: public Danvil::Print::IPrintable
{
public:
	typedef K ScalarType;
	static const int cCartesianDimension = N_CAT;
	static const int cNumberOfRotations = N_ROT;

public:
	TCState<K, N_CAT> sc;
	TRState<K> sr[N_ROT];

public:
	unsigned int dimension() const {
		return sc.dimension() + N_ROT * sr[0].dimension();
	}

	size_t numbersCount() const {
		return sc.numbersCount() + N_ROT * sr[0].numbersCount();
	}

	void toNumbers(size_t n, K* data) const {
		assert(n == numbersCount());
		sc.toNumbers(sc.numbersCount(), data);
		data += sc.numbersCount();
		for(int i=0; i<N_ROT; i++) {
			size_t len = sr[i].numbersCount();
			sr[i].toNumbers(len, data);
			data += len;
		}
	}

	void fromNumbers(size_t n, const K* data) {
		assert(n == numbersCount());
		sc.fromNumbers(sc.numbersCount(), data);
		data += sc.numbersCount();
		for(int i=0; i<N_ROT; i++) {
			size_t len = sr[i].numbersCount();
			sr[i].fromNumbers(len, data);
			data += len;
		}
	}

	void print(std::ostream& os) const {
		os << "[" << sc << ", ";
		for(int i=0; i<N_ROT; i++) {
			os << sr[i] << ", ";
		}
		os << "]";
	}

};

template<typename K, int N_CAT, int N_ROT>
class TCRStateOperator
{
public:
	typedef TCRState<K, N_CAT, N_ROT> State;
	typedef TCStateOperator<K, N_CAT> COP;
	typedef TRStateOperator<K> ROP;

public:
	static K Distance(const State& a, const State& b) {
		K d = COP::Distance(a.sc, b.sc);
		for(int i=0; i<N_ROT; i++) {
			d += ROP::Distance(a.sr[i], b.sr[i]);
		}
	}

	static State Compose(const State& a, const State& b) {
		State c;
		c.sc = COP::Compose(a.sc, b.sc);
		for(int i=0; i<N_ROT; i++) {
			c.sr[i] = ROP::Compose(a.sr[i], b.sr[i]);
		}
		return c;
	}

	static State Difference(const State& a, const State& b) {
		State c;
		c.sc = COP::Difference(a.sc, b.sc);
		for(int i=0; i<N_ROT; i++) {
			c.sr[i] = ROP::Difference(a.sr[i], b.sr[i]);
		}
		return c;
	}

	static State WeightedSum(K f1, const State& s1, K f2, const State& s2, K f3, const State& s3) {
		State c;
		c.sc = COP::WeightedSum(f1, s1.sc, f2, s2.sc, f3, s3.sc);
		for(int i=0; i<N_ROT; i++) {
			c.sr[i] = ROP::WeightedSum(f1, s1.sr[i], f2, s2.sr[i], f3, s3.sr[i]);
		}
		return c;
	}

	static State Mean(const std::vector<State>& states) {
		return WeightedSum(std::vector<K>(states.size(), (K)1), states);
	}

};

template<typename K, int N_CAT, int N_ROT>
class TCRDomain
{
public:
	typedef TCRState<K, N_CAT, N_ROT> State;
	typedef TCDomainBox<K, N_CAT> COM;
	typedef Danvil::ctLinAlg::Vec<K, N_CAT> V;
	typedef TRFull<K> ROM;

public:
	COM com;
	ROM rom[N_ROT];

	TCRDomain(const V& min, const V& max)
	: com(min, max) {}

	State random() const {
		State s;
		s.sc = com.random();
		for(int i=0; i<N_ROT; i++) {
			s.sr[i] = rom[i].random();
		}
		return s;
	}

	State random(const State& center, const std::vector<K>& noise) {
		if(noise.size() != N_CAT + N_ROT) {
			throw std::runtime_error("Noise vector has invalid length!");
		}
		State s;
		s.sc = com.random(center.sc, std::vector<K>(noise.begin(), noise.begin() + N_CAT));
		for(int i=0; i<N_ROT; i++) {
			s.sr[i] = rom[i].random(center.sr[i], noise[N_CAT + i]);
		}
		return s;
	}

	std::vector<State> random(size_t n) const {
		std::vector<State> states;
		for(size_t i=0; i<n; i++) {
			states.push_back(random());
		}
		return states;
	}

	State project(const State& x) const {
		State s;
		s.sc = com.project(x.sc);
		for(int i=0; i<N_ROT; i++) {
			s.sr[i] = rom[i].project(x.sr[i]);
		}
		return s;
	}


};

#endif
