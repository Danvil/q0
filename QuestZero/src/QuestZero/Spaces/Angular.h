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
#include <Danvil/Tools/MoreMath.h>
#include <Danvil/Tools/Field.h>
#include <Danvil/Print.h>
#include <vector>
#include <cassert>
//---------------------------------------------------------------------------
namespace Q0 {
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
				return (double)Danvil::MoreMath::Min(w, Danvil::C_2_PI - w);
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
				// FIXME this is incorrect!
				return Wrap((K)f1 * Wrap(s1) + (K)f2 * Wrap(s2));
			}

			template<typename S>
			K weightedSum(S f1, K s1, S f2, K s2, S f3, K s3) const {
				// FIXME this is incorrect!
				return Wrap((K)f1 * Wrap(s1) + (K)f2 * Wrap(s2) + (K)f3 * Wrap(s3));
			}

			template<typename S>
			K weightedSum(const std::vector<S>& factors, const std::vector<K>& states) const {
				if(factors.size() != states.size()) {
					// Number of factors and states must be equal!
					throw WeightedSumException();
				}
				// FIXME this is incorrect!
				K ws = (K)0;
				for(size_t i=0; i<states.size(); ++i) {
					ws += (K)factors[i] * Wrap(states[i]);
				}
				return Wrap(ws);
			}

			/** Restrict the angle to [0,2Pi] */
			static K Wrap(K x) {
				return Danvil::MoreMath::Wrap(x, (K)Danvil::C_2_PI);
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
				return RandomNumbers::Uniform<K>() * Danvil::C_2_PI;
			}

			template<typename NT>
			State random(const State& center, const std::vector<NT>& noise) const {
				assert(noise.size() == dimension());
				NT r = RandomNumbers::UniformMP(noise[0]);
				return project(center + (K)r);
			}

		protected:
			~Full() {}
		};

		/**
		 * lower < upper represents the interval [lower|upper]
		 * upper < lower represents the intervals [0|upper] and [lower|2Pi]
		 */
		template<typename K>
		struct Interval
		{
			// FIXME enforce float or double for K
			Interval()
			{}

			Interval(K lower, K upper)
			: lower_(lower),
			  upper_(upper) {}

			size_t dimension() const { return 1; }

			K project(K x) const {
				x = Danvil::MoreMath::Wrap(x, (K)Danvil::C_2_PI);
				// project to nearest border
				if(lower_ < upper_) {
					// normal case: allowed is [lower|upper]
					if(Danvil::MoreMath::InInterval(x, lower_, upper_)) {
						// already in interval
						return x;
					} else {
						// be careful to map to the right bound
						K d1 = lower_;
						K d2 = (K)Danvil::C_2_PI - upper_;
						if(d1 < d2) {
							// the mid point of the excluded area lies in the right interval
							K mu = upper_ + (d2 - d1) / 2;
							return Danvil::MoreMath::InInterval(x, upper_, mu) ? upper_ : lower_;
						} else {
							// the mid point of the excluded area lies in the left interval
							K mu = (d1 - d2) / 2;
							return Danvil::MoreMath::InInterval(x, mu, lower_) ? lower_ : upper_;
						}
					}
				} else {
					// two-interval case: allowed is [0|upper] and [lower|2Pi]
					if(Danvil::MoreMath::InInterval(x, upper_, lower_)) {
						// already in interval
						return x;
					} else {
						// mid point of excluded interval which is [upper|lower]
						K mu = (lower_ - upper_) / 2;
						return Danvil::MoreMath::InInterval(x, upper_, mu) ? upper_ : lower_;
					}
				}
			}

			K random() const {
				if(lower_ < upper_) {
					return RandomNumbers::Uniform<K>(lower_, upper_);
				} else {
					K r = RandomNumbers::Uniform<K>(lower_, upper_ + (K)Danvil::C_2_PI);
					return Danvil::MoreMath::Wrap(r, (K)Danvil::C_2_PI);
				}
			}

			template<typename NT>
			K random(K center, const std::vector<NT>& noise) const {
				assert(noise.size() == dimension());
				K r = center + RandomNumbers::UniformMP(noise[0]);
				return project(r);
			}

			DEFINE_FIELD(lower, K)
			DEFINE_FIELD(upper, K)
		};

	}

	//---------------------------------------------------------------------------

	template<
		typename State,
		class Domain = Domains::Full<State>,
		class Operator = Operations::AngularOps<State>
	>
	struct AngularSpace
	: public BaseSpace<State, Operator, Domain>
	{ };

	template<typename State>
	struct FullAngularSpace
	: public AngularSpace<State, Domains::Full<State> >
	{ };

	template<typename State>
	struct IntervalAngularSpace
	: public AngularSpace<State, Domains::Interval<State> >
	{ };

}

//---------------------------------------------------------------------------
}}
//---------------------------------------------------------------------------
#endif
