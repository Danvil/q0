/*
 * Cartesian.h
 *
 *  Created on: Sep 10, 2010
 *      Author: david
 */

#ifndef SPACE_CARTESIAN_H_
#define SPACE_CARTESIAN_H_
//---------------------------------------------------------------------------
#include "Space.h"
#include "../RandomNumbers.h"
#include <Danvil/LinAlg.h>
#include <Danvil/Memops/Copy.h>
#include <vector>
#include <cassert>
//---------------------------------------------------------------------------
namespace Spaces { namespace Cartesian {
//---------------------------------------------------------------------------

namespace Operations
{
	template<typename State>
	struct Linear
	{
		typedef typename State::ScalarType K;

		struct WeightedSumException {};

		K distance(const State& a, const State& b) const {
			return Danvil::ctLinAlg::Distance(a, b);
		}

		State inverse(const State& a) const {
			return State(-a.cartesian);
		}

		State compose(const State& a, const State& b) const {
			return State(a.cartesian + b.cartesian);
		}

		State difference(const State& a, const State& b) const {
			return State(a.cartesian - b.cartesian);
		}

		State weightedSum(K f1, const State& s1, K f2, const State& s2) const {
			return State(f1 * s1.cartesian + f2 * s2.cartesian);
		}

		State weightedSum(K f1, const State& s1, K f2, const State& s2, K f3, const State& s3) const {
			return State(f1 * s1.cartesian + f2 * s2.cartesian + f3 * s3.cartesian);
		}

		State weightedSum(const std::vector<K>& factors, const std::vector<State>& states) const {
			if(factors.size() != states.size()) {
				// Number of factors and states must be equal!
				throw WeightedSumException();
			}
			if(states.size() == 0) {
				// Must have at least one element for WeightedSum!
				throw WeightedSumException();
			}
			State c = factors[0] * states[0];
			for(size_t i=1; i<states.size(); i++) {
				c += factors[i] * states[i];
			}
			return c;
		}
	};
}

//---------------------------------------------------------------------------

namespace Domains
{
	// TODO: allow states with variable dimension?
	template<typename State>
	struct Box
	{
		typedef typename State::ScalarType K;

		struct InvalidNoiseVectorException {};

		Box() {}

		Box(const State& min, const State& max)
		: _min(min),
		  _max(max) {}

		void setDomainRange(const State& range) {
			_min = -range;
			_max = range;
		}

		void setDomainMin(const State& min) {
			_min = min;
		}

		void setDomainMax(const State& max) {
			_max = max;
		}

		size_t dimension() const {
			return State::Dimension;
		}

		State project(const State& s) const {
			State v;
			for(size_t i=0; i<dimension(); i++) {
				v[i] = Danvil::clamped(s[i], _min[i], _max[i]);
			}
			return v;
		}

		State random() const {
			return RandomV(_min, _max);
		}

		State random(const State& center, const std::vector<K>& noise) {
			if(noise.size() != State::Dimension) {
				throw InvalidNoiseVectorException();
			}
			State noise_range(noise.data());
			return project(this->compose(center, RandomV(-noise_range, noise_range)));
		}

	private:
		static State RandomV(const State& min, const State& max) {
			State v;
			for(size_t i=0; i<v.dimension(); i++) {
				v[i] = RandomNumbers::S.random(min[i], max[i]);
			}
			return v;
		}

		State _min, _max;
	};
}

//---------------------------------------------------------------------------

template<
	typename State,
	class Operator = Operations::Linear<State>,
	class Domain = Domains::Box<State>,
	class OperationFinal = OperationFinalPolicy::Projected<State>
>
struct CartesianSpace
: public BaseSpace<State, Operator, Domain, OperationFinal>
{ };

//---------------------------------------------------------------------------
}}
//---------------------------------------------------------------------------
#endif
