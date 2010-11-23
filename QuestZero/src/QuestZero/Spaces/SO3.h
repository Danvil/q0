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
#include <Danvil/SO3.h>
#include <Danvil/Memops/Copy.h>
#include <Danvil/Tools/MoreMath.h>
#include <Danvil/Tools/Field.h>
#include <Danvil/Print.h>
#include <Danvil/Ptr.h>
#include <boost/foreach.hpp>
#include <boost/range.hpp>
#include <vector>
#include <cassert>
#include <stdexcept>
//---------------------------------------------------------------------------
namespace Q0 {
namespace Spaces {
//---------------------------------------------------------------------------

namespace SO3 {

	namespace Operations
	{
		template<typename K>
		struct SO3Ops
		{
			typedef Danvil::SO3::Quaternion<K> State;

			struct WeightedSumException {};

			double distance(const State& a, const State& b) const {
				return (double)State::Distance(a, b);
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
				return Danvil::SO3::RotationTools::WeightedMean(states, factors, (K)1e-3);
			}

		protected:
			~SO3Ops() {}
		};
	}

	//---------------------------------------------------------------------------

	namespace Domains
	{
		template<typename K>
		struct Full
		{
			typedef Danvil::SO3::Quaternion<K> State;

			size_t dimension() const {
				return 3;
			}

			State project(const State& s) const {
				State cp = s;
				cp.normalize();
				return cp;
			}

			State random() const {
				return Danvil::SO3::RotationTools::UniformRandom<K>(&RandomNumbers::Uniform<K>);
			}

			template<typename NT>
			State random(const State& center, const std::vector<NT>& noise) const {
				assert(noise.size() == dimension());
				K d = (K)noise[0];
				return center * Danvil::SO3::RotationTools::UniformRandom<K>(d, &RandomNumbers::Uniform<K>);
			}

		protected:
			~Full() {}
		};

		template<typename K>
		struct MaxDistance
		{
			typedef Danvil::SO3::Quaternion<K> State;

			MaxDistance(K range = Danvil::Pi<K>())
			: range_(range) {}

			size_t dimension() const {
				return 3;
			}

			State project(const State& s) const {
				// use a quaternion which has at most an rotation angle of PI
				State r = (s.w >= 0) ? s : -s;
				double angle = 2 * std::acos(r.w); // is in [0,PI], because r.w > 0!
				if(angle < range_) {
					return r;
				} else {
					// reduce rotation by creating a rotation of maximal angle and given axis
					return State::FactorVectorAngle(r.x, r.y, r.z, range_);
				}
			}

			State random() const {
				return Danvil::SO3::RotationTools::UniformRandom<K>(range_, &RandomNumbers::Uniform<K>);
			}

			template<typename NT>
			State random(const State& center, const std::vector<NT>& noise) const {
				assert(noise.size() == dimension());
				K d = (K)noise[0];
				State s = center * Danvil::SO3::RotationTools::UniformRandom<K>(d, &RandomNumbers::Uniform<K>);
				return project(s);
			}

			DEFINE_FIELD(range, K)

		protected:
			~MaxDistance() {}
		};
	}

	//---------------------------------------------------------------------------

	template<
		typename K,
		class Operator = Operations::SO3Ops<K>,
		class Domain = Domains::Full<K>,
		class OperationFinal = OperationFinalPolicy::Unprojected<Danvil::SO3::Quaternion<K> >
	>
	struct SO3Space
	: public BaseSpace<Danvil::SO3::Quaternion<K>, Operator, Domain, OperationFinal>
	{ };
}

//---------------------------------------------------------------------------
}}
//---------------------------------------------------------------------------
#endif
