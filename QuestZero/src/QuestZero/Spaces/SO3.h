/*
 * SO3.h
 *
 *  Created on: Sep 10, 2010
 *      Author: david
 */

#ifndef SPACE_SO3_H_
#define SPACE_SO3_H_
//---------------------------------------------------------------------------
#include "BaseSpace.h"
#include "QuestZero/Common/RandomNumbers.h"
#include <Danvil/LinAlg.h>
#include <Danvil/LinAlg/RotationTools.h>
#include <Danvil/Memops/Copy.h>
#include <Danvil/Tools/Small.h>
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

namespace SO3 {

	namespace Operations
	{
		template<typename K>
		struct SO3Ops
		{
			typedef Danvil::ctLinAlg::TQuaternion<K> State;

			struct WeightedSumException {};

			K distance(const State& a, const State& b) const {
				return State::Distance(a, b);
			}

			State inverse(const State& a) const {
				return a.inverse();
			}

			State compose(const State& a, const State& b) const {
				return a * b;
			}

			// TODO this is the default implementation
			State difference(const State& a, const State& b) const {
				return compose(a, inverse(b));
			}

			// TODO this is the default implementation
			template<typename S>
			State weightedSum(S f1, const State& s1, S f2, const State& s2) const {
				std::vector<S> factors;
				factors.push_back(f1);
				factors.push_back(f2);
				std::vector<State> states;
				states.push_back(s1);
				states.push_back(s2);
				return weightedSum(factors, states);
			}

			// TODO this is the default implementation
			template<typename S>
			State weightedSum(S f1, const State& s1, S f2, const State& s2, S f3, const State& s3) const {
				std::vector<S> factors;
				factors.push_back(f1);
				factors.push_back(f2);
				factors.push_back(f3);
				std::vector<State> states;
				states.push_back(s1);
				states.push_back(s2);
				states.push_back(s3);
				return weightedSum(factors, states);
			}

			template<typename S>
			State weightedSum(const std::vector<S>& factors, const std::vector<State>& states) const {
				if(factors.size() != states.size()) {
					// Number of factors and states must be equal!
					throw WeightedSumException();
				}
				return Danvil::ctLinAlg::RotationTools::WeightedMean(states, factors, (K)1e-3);
			}

		protected:
			~SO3Ops() {}
		};
	}

	//---------------------------------------------------------------------------

	namespace Domains
	{
		// TODO: allow states with variable dimension?
		template<typename K>
		struct Full
		{
			typedef Danvil::ctLinAlg::TQuaternion<K> State;

			Full() {}

			size_t dimension() const {
				return 3;
			}

			State project(const State& s) const {
				State cp = s;
				cp.normalize();
				return cp;
			}

			State random() const {
				return Danvil::ctLinAlg::RotationTools::UniformRandom<K>(&RandomNumbers::Random01);
			}

			template<typename NT>
			State random(const State& center, const std::vector<NT>& noise) const {
				assert(noise.size() == dimension());
				K d = (K)noise[0];
				return center * Danvil::ctLinAlg::RotationTools::UniformRandom<K>(d, &RandomNumbers::Random01);
			}

		protected:
			~Full() {}
		};
	}

	//---------------------------------------------------------------------------

	template<
		typename K,
		class Operator = Operations::SO3Ops<K>,
		class Domain = Domains::Full<K>,
		class OperationFinal = OperationFinalPolicy::Unprojected<Danvil::ctLinAlg::TQuaternion<K> >
	>
	struct SO3Space
	: public BaseSpace<Danvil::ctLinAlg::TQuaternion<K>, Operator, Domain, OperationFinal>
	{ };
}

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
