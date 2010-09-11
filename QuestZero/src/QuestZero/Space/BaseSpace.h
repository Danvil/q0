/*
 * Space.h
 *
 *  Created on: Sep 10, 2010
 *      Author: david
 */

#ifndef SPACE_H_
#define SPACE_H_
//---------------------------------------------------------------------------
#include "OperationFinalPolicy.h"
#include <vector>
//---------------------------------------------------------------------------
namespace Spaces {
//---------------------------------------------------------------------------

template<
	typename _State,
	class Operator,
	class Domain,
	class OperationFinal = OperationFinalPolicy::Projected<_State>
>
struct BaseSpace
: public Operator,
  public OperationFinal,
  public Domain
{
	typedef _State State;

	typedef typename State::ScalarType K;

	/** Distance between two states */
	K distance(const State& a, const State& b) const {
		return opFinal(*this, Operator::distance(a, b));
	}

	/** Product of two states */
	State inverse(const State& a) const {
		return opFinal(*this, Operator::inverse(a));
	}

	/** Product of two states */
	State compose(const State& a, const State& b) const {
		return opFinal(*this, Operator::compose(a, b));
	}

	/** Difference between two states */
	State difference(const State& a, const State& b) const {
		return opFinal(*this, Operator::difference(a, b));
	}

	/** Weighted sum of two states */
	State weightedSum(K f1, const State& s1, K f2, const State& s2) const {
		return opFinal(*this, Operator::weightedSum(f1, s1, f2, s2));
	}

	/** Weighted sum of three states */
	State weightedSum(K f1, const State& s1, K f2, const State& s2, K f3, const State& s3) const {
		return opFinal(*this, Operator::weightedSum(f1, s1, f2, s2, f3, s3));
	}

	/** Weighted sum of arbitrary many states */
	State weightedSum(const std::vector<K>& factors, const std::vector<State>& states) const {
		return opFinal(*this, Operator::weightedSum(factors, states));
	}

	/** Weights all states equally */
	State mean(const std::vector<State>& states) const {
		return weightedSum(std::vector<K>(states.size(), (K)1), states);
	}

	/** Get several random states */
	std::vector<State> randomMany(size_t n) const {
		std::vector<State> states;
		for(size_t i=0; i<n; i++) {
			states.push_back(this->random());
		}
		return states;
	}


};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
