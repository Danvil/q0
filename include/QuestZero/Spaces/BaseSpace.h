/*
 * Space.h
 *
 *  Created on: Sep 10, 2010
 *      Author: david
 */

#ifndef SPACE_H_
#define SPACE_H_
//---------------------------------------------------------------------------
#include "QuestZero/Policies/OperationFinalPolicy.h"
#include <QuestZero/Common/IPrintable.h>
#include <vector>
#include <cstddef>
//---------------------------------------------------------------------------
namespace Q0 {
namespace Spaces {
//---------------------------------------------------------------------------

template<
	typename State_,
	class Operator,
	class Domain,
	class OperationFinal = OperationFinalPolicy::Projected<State_>
>
struct BaseSpace
: public Operator,
  public Domain,
  public OperationFinal,
  public IPrintable
{
	typedef State_ State;

	typedef double K;

	virtual void print(std::ostream& os) const {
		os << "BaseSpace";
	}

	/** Distance between two states */
	K distance(const State& a, const State& b) const {
		return opFinal(*this, Operator::distance(a, b));
	}

	/** Scales a state element */
	State scale(const State& a, double s) const {
		return opFinal(*this, Operator::scale(a, s));
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
	template<typename K>
	State weightedSum(K f1, const State& s1, K f2, const State& s2) const {
		return opFinal(*this, Operator::weightedSum(f1, s1, f2, s2));
	}

	/** Weighted sum of three states */
	template<typename K>
	State weightedSum(K f1, const State& s1, K f2, const State& s2, K f3, const State& s3) const {
		return opFinal(*this, Operator::weightedSum(f1, s1, f2, s2, f3, s3));
	}

	/** Weighted sum of arbitrary many states */
	template<typename K>
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

template<typename State, class Operator, class Domain, class OperationFinal>
std::ostream& operator<<(std::ostream& os, const BaseSpace<State,Operator,Domain,OperationFinal>& state) {
	state.print(os);
	return os;
}


//---------------------------------------------------------------------------
}}
//---------------------------------------------------------------------------
#endif
