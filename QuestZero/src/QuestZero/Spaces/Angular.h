/*
 * Angular.h
 *
 *  Created on: Sep 10, 2010
 *      Author: david
 */

#ifndef SPACE_ANGULAR_H_
#define SPACE_ANGULAR_H_
//---------------------------------------------------------------------------
#include "BaseSpace.h"
#include "QuestZero/Common/RandomNumbers.h"
#include <Danvil/Tools/Small.h>
#include <Danvil/Print.h>
#include <cassert>
//---------------------------------------------------------------------------
namespace Spaces {
//---------------------------------------------------------------------------

/** Operations and domains for a 2D rotation
 * The state vector is expected to be a floating point number.
 */
namespace Angular {

	namespace Operations
	{
		template<typename State>
		struct AngularOps
		{
			// FIXME assure that State is float or double

			typedef State K;

			struct WeightedSumException {};

			double distance(const K& a, const K& b) const {
				// compute shortest distance!
				K w = Wrap(a - b);
				return (double)Danvil::min(w, Danvil::C_2_PI - w);
			}

			K inverse(const K& a) const {
				return Wrap(-a);
			}

			K compose(const K& a, const K& b) const {
				return Wrap(a + b);
			}

			K difference(const K& a, const K& b) const {
				return Wrap(a - b);
			}

			template<typename S>
			K weightedSum(S f1, K s1, S f2, K s2) const {
				// FIXME prove that this is mathematically correct?
				return Wrap((K)f1 * Wrap(s1) + (K)f2 * Wrap(s2));
			}

			template<typename S>
			K weightedSum(S f1, K s1, S f2, K s2, S f3, K s3) const {

				// FIXME prove that this is mathematically correct?
				return Wrap((K)f1 * Wrap(s1) + (K)f2 * Wrap(s2) + (K)f3 * Wrap(s3));
			}

			template<typename S>
			K weightedSum(const std::vector<S>& factors, const std::vector<K>& states) const {
				if(factors.size() != states.size()) {
					// Number of factors and states must be equal!
					throw WeightedSumException();
				}
				// FIXME prove that this is mathematically correct?
				K ws = (K)0;
				for(size_t i=0; i<states.size(); ++i) {
					ws += (K)factors[i] * Wrap(states[i]);
				}
				return Wrap(ws);
			}

			/** Restrict the angle to [-Pi,+Pi] */
			static K Wrap(K x) {
//				static const K delta = (K)Danvil::C_2_PI;
//				x = fmod(x, delta);
//				return (x < 0) ? x + delta : x;
				static const K pi = (K)Danvil::C_PI;
				x = fmod(x, 2*pi);
				return (x < 0) ? (x + pi) : (x - pi);
				// same as ((x<0)?(x+2*pi):(x)) - pi;
			}

		protected:
			~AngularOps() {}
		};
	}

	//---------------------------------------------------------------------------

	namespace Domains
	{
		// TODO: allow states with variable dimension?
		template<typename K>
		struct Full
		{
			typedef K State;

			size_t dimension() const {
				return 1;
			}

			State project(const State& s) const {
				return Operations::AngularOps<K>::Wrap(s);
			}

			State random() const {
				// random number in [0, 2*Pi]
				return RandomNumbers::Random01() * Danvil::C_2_PI;
			}

			template<typename NT>
			State random(const State& center, const std::vector<NT>& noise) const {
				assert(noise.size() == dimension());
				NT r = RandomNumbers::S.randomMP(noise[0]);
				return project(center + (K)r);
			}

		protected:
			~Full() {}
		};
	}

	//---------------------------------------------------------------------------

	template<
		typename State,
		class Operator = Operations::AngularOps<State>,
		class Domain = Domains::Full<State>
	>
	struct AngularSpace
	: public BaseSpace<State, Operator, Domain>
	{ };
}

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
