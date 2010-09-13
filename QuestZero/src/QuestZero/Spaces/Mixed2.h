/*
 * Mixed2.h
 *
 *  Created on: Sep 11, 2010
 *      Author: david
 */

#ifndef SPACE_MIXED2_H_
#define SPACE_MIXED2_H_
//---------------------------------------------------------------------------
#include <Danvil/Print.h>
#include <Danvil/Ptr.h>
#include <boost/foreach.hpp>
#include <boost/range.hpp>
#include <vector>
#include <cassert>
#include <stdexcept>
//---------------------------------------------------------------------------
namespace Spaces {
//---------------------------------------------------------------------------

template<class Space1, unsigned int N, class Space2>
struct Mixed2State
: public Danvil::Print::IPrintable
{
	static const unsigned int SpaceCount = 2;

	typedef typename Space1::State State1;

	typedef typename Space2::State State2;

	State1 sc;

	State2 sr[N];

	void print(std::ostream& os) const {
		os << "[" << sc << ", ";
		for(unsigned int i=0; i<N; ++i) {
			os << sr[i];
			if(i != N - 1) {
				os << ", ";
			}
		}
		os << "]";
	}
};

//---------------------------------------------------------------------------

template<class Space1, unsigned int N_ROT, class Space2>
struct Mixed2Space
{
	static const unsigned int SpaceCount = 2;

	typedef Mixed2State<Space1, N_ROT, Space2> State;

	typedef typename State::State1 State1;

	typedef typename State::State2 State2;

	typedef typename State1::ScalarType K;
	// FIXME check/assure that State1 scalar is identical to State2 scalar

	void setSpace1(const Space1& cs) {
		_c_space = cs;
	}

	void setSpace2(unsigned int i, const Space2& s) {
		_r_space[i] = s;
	}

	void setSpace2All(const Space2& s) {
		for(unsigned int i=0; i<N_ROT; ++i) {
			setSpace2(i, s);
		}
	}

	K distance(const State& a, const State& b) const {
		K d = (K)0;
		d += _c_space.inverse(a.sc);
		for(unsigned int i=0; i<N_ROT; ++i) {
			d += _r_space[i].distance(a.sr[i], b.sr[i]);
		}
		return d;
	}

	State inverse(const State& a) const {
		State x;
		x.sc = _c_space.inverse(a.sc);
		for(unsigned int i=0; i<N_ROT; ++i) {
			x.sr[i] = _r_space[i].inverse(a.sr[i]);
		}
		return x;
	}

	State compose(const State& a, const State& b) const {
		State x;
		x.sc = _c_space.compose(a.sc, b.sc);
		for(unsigned int i=0; i<N_ROT; ++i) {
			x.sr[i] = _r_space[i].compose(a.sr[i], b.sr[i]);
		}
		return x;
	}

	// TODO this is the default implementation
	State difference(const State& a, const State& b) const {
		State x;
		x.sc = _c_space.difference(a.sc, b.sc);
		for(unsigned int i=0; i<N_ROT; ++i) {
			x.sr[i] = _r_space[i].difference(a.sr[i], b.sr[i]);
		}
		return x;
	}

	// TODO this is the default implementation
	State weightedSum(K f1, const State& s1, K f2, const State& s2) const {
		State c;
		c.sc = _c_space.weightedSum(f1, s1.sc, f2, s2.sc);
		for(unsigned int i=0; i<N_ROT; ++i) {
			c.sr[i] = _r_space.weightedSum(f1, s1.sr[i], f2, s2.sr[i]);
		}
		return c;
	}

	// TODO this is the default implementation
	State weightedSum(K f1, const State& s1, K f2, const State& s2, K f3, const State& s3) const {
		State c;
		c.sc = _c_space.weightedSum(f1, s1.sc, f2, s2.sc, f3, s3.sc);
		for(unsigned int i=0; i<N_ROT; ++i) {
			c.sr[i] = _r_space[i].weightedSum(f1, s1.sr[i], f2, s2.sr[i], f3, s3.sr[i]);
		}
		return c;
	}

	State weightedSum(const std::vector<K>& factors, const std::vector<State>& states) const {
		State c;
		std::vector<State1> states1;
		states1.reserve(states.size());
		for(size_t i=0; i<states.size(); ++i) {
			states1[i] = states[i].sc;
		}
		c.sc = _c_space.weightedSum(factors, states1);
		for(unsigned int j=0; j<N_ROT; ++j) {
			std::vector<State2> states2;
			states2.reserve(states.size());
			for(size_t i=0; i<states.size(); ++i) {
				states2[i] = states[i].sr[j];
			}
			c.sr[j] = _r_space.weightedSum(factors, states2);
		}
		return c;
	}

	/** Weights all states equally */
	State mean(const std::vector<State>& states) const {
		return weightedSum(std::vector<K>(states.size(), (K)1), states);
	}

	/** Get several random states */
	std::vector<State> randomMany(size_t n) const {
		std::vector<State> states;
		for(size_t i=0; i<n; ++i) {
			states.push_back(this->random());
		}
		return states;
	}

	State random() const {
		State s;
		s.sc = _c_space.random();
		for(unsigned int i=0; i<N_ROT; ++i) {
			s.sr[i] = _r_space[i].random();
		}
		return s;
	}

	State random(const State& center, const std::vector<K>& noise) const {
		if(noise.size() != Space1::Dimension + N_ROT) {
			throw std::runtime_error("Noise vector has invalid length!");
		}
		State s;
		s.sc = _c_space.random(center.sc, std::vector<K>(noise.begin(), noise.begin() + Space1::Dimension));
		for(int i=0; i<N_ROT; ++i) {
			s.sr[i] = _r_space[i].random(center.sr[i], noise[Space1::Dimension + i]);
		}
		return s;
	}

	State project(const State& x) const {
		State s;
		s.sc = _c_space.project(x.sc);
		for(unsigned int i=0; i<N_ROT; ++i) {
			s.sr[i] = _r_space[i].project(x.sr[i]);
		}
		return s;
	}

protected:
	Space1 _c_space;

	Space2 _r_space[N_ROT];

};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
