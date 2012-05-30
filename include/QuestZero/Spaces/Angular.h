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
#include "QuestZero/Common/Exceptions.h"
#include <Danvil/Tools/MoreMath.h>
#include <Danvil/Tools/Constants.h>
#include <Danvil/Tools/Field.h>
#include <Danvil/SO3.h>
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

			double distance(K a, K b) const {
				// compute shortest distance!
				K w = Wrap(a - b);
				return (double)Danvil::MoreMath::Min(w, Danvil::C_2_PI - w);
			}

			K scale(K a, double s) const {
				return Wrap(K(s) * a);
			}

			K inverse(K a) const {
				return Wrap(-a);
			}

			K compose(K a, K b) const {
				return Wrap(a + b);
			}

			K difference(K a, K b) const {
				return Wrap(a - b);
			}

			template<typename S>
			K weightedSum(S f1, K s1, S f2, K s2) const {
				std::vector<S> factors;
				factors.push_back(f1);
				factors.push_back(f2);
				std::vector<State> states;
				states.push_back(s1);
				states.push_back(s2);
				return weightedSum(factors, states);
			}

			template<typename S>
			K weightedSum(S f1, K s1, S f2, K s2, S f3, K s3) const {
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
			K weightedSum(const std::vector<S>& factors, const std::vector<K>& states) const {
				INVALID_SIZE_EXCEPTION(factors.size() != states.size()) // Number of factors and states must be equal!
				return Danvil::SO3::RotationTools::WeightedMean(states, factors, (K)1e-3);
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

			State zero() const {
				return State(0);
			}

			State unit(size_t) const {
				return State(1);
			}

			template<typename SCL>
			State unit(size_t, SCL s) const {
				return State(s);
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

		private:
			/** The used computation range is [0,R] (R could 2*pi or 1) */
			// FIXME why can't we use Danvil::C_2_PI??
			static constexpr K cRange = K(2.0 * 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709384460955058223172535940812848111);

			/** Wraps the angle to the used computation range */
			static K wrap(K x) {
				// TODO improve performance?
				return Danvil::MoreMath::Wrap(x, cRange);
			}

		public:
			/** Creates a space which allows all values */
			Interval() {
				set_lower(K(0));
				set_upper(cRange);
			}

			/** Creates a space which allows only values in the given interval
			 * See set_lower and set_upper for more information
			 */
			Interval(K lower, K upper)  {
				set_lower(lower);
				set_upper(upper);
			}

			/** Angular space has a dimension of 1 */
			size_t dimension() const {
				return 1;
			}

			K zero() const {
				return K(0);
			}

			K unit(unsigned int) const {
				return K(1);
			}

			template<typename SCL>
			K unit(unsigned int, SCL s) const {
				return K(s);
			}

			/** Returns true if the value is in the allowed interval */
			bool contains(K x) const {
				// wrap to range before testing
				x = wrap(x);
				if(lower_ < upper_) {
					// test if in interval
					return Danvil::MoreMath::InInterval(x, lower_, upper_);
				} else {
					// test if not in not allowed interval
					return !Danvil::MoreMath::InInterval(x, upper_, lower_);
				}
			}

			/** Projects a value to the allowed interval
			 * If the value is outside of the allowed interval, the nearest
			 * interval border is returned.
			 */
			K project(K x) const {
				// wrap to range before projecting
				x = wrap(x);
				// project to nearest border
				if(lower_ < upper_) {
					// normal case: allowed is [lower|upper]
					if(Danvil::MoreMath::InInterval(x, lower_, upper_)) {
						// already in interval
						return x;
					}
					else {
						// be careful to map to the right bound
						K d1 = lower_; // length of forbidden interval to the left
						K d2 = cRange - upper_; // length of forbidden interval to the right
						K mean = (d1 + d2) / 2; // mean length of forbidden interval
						if(d1 < d2) {
							// the mid point of the excluded area lies in the right interval
							// 0 --> lower +++++++ upper <---- mu -- R
							return Danvil::MoreMath::InInterval(x, upper_, upper_ + mean) ? upper_ : lower_;
						}
						else {
							// the mid point of the excluded area lies in the left interval
							// 0 -- mu ----> lower +++++++ upper <-- R
							return Danvil::MoreMath::InInterval(x, lower_ - mean, lower_) ? lower_ : upper_;
						}
					}
				}
				else {
					// two-interval case: allowed is [0|upper] and [lower|R]
					if(!Danvil::MoreMath::InInterval(x, upper_, lower_)) {
						// already in interval
						return x;
					}
					else {
						// 0 ++++ upper <-- mu --> lower +++ R
						// compute mid point of excluded interval which is [upper|lower]
						K mu = (lower_ + upper_) / 2;
						// map to the nearer bound
						return Danvil::MoreMath::InInterval(x, upper_, mu) ? upper_ : lower_;
					}
				}
			}

			/** Draws a random uniformly distributed value out of the allowed interval */
			K random() const {
				if(lower_ < upper_) {
					// sample in allowed interval
					// 0 ------- lower xxxxxxx upper --- R
					return RandomNumbers::Uniform<K>(lower_, upper_);
				}
				else {
					// sample outside of range to have a continuous interval
					// 0 ++++ upper ------ lower xxxxx R xxxx R+upper ----...
					K r = RandomNumbers::Uniform<K>(lower_, cRange + upper_);
					// wrap back
					//return wrap(r);
					if(r >= cRange) {
						r -= cRange;
					}
					return r;
				}
			}

			/** Draws a random uniformly distributed value centered around the given value out of the allowed interval */
			template<typename NT>
			K random(K center, NT noise) const {
				const size_t cMaxTrials = 100;
				// FIXME remove the loop by exactly computing the allowed interval
				size_t trials = 0;
				while(trials < cMaxTrials) {
					// add a random uniformly distributed offset in the given noise range
					K x = center + RandomNumbers::UniformMP(noise);
					// wrap to the [0,2pi] interval
					x = wrap(x);
					// if it is in the interval we are ready
					if(contains(x)) {
						return x;
					}
					// else try again
					trials++;
				}
				//assert(false); // intervals possibly do not overlap
				return project(center);
//				if(lower_ < upper_) {
//					//  -[0,a[-  +[a,b]+  -]b,2pi[-
//					K r = center + RandomNumbers::UniformMP(noise);
//					return project(r);
//				} else {
//
//				}
			}

			template<typename NT>
			K random(K center, const std::vector<NT>& noise) const {
				assert(noise.size() == dimension());
				return random(center, noise[0]);
			}

			K lower() const {
				return lower_;
			}

			K upper() const {
				return upper_;
			}

			/** Sets lower value of allowed range
			 * If lower < upper the allowed range is:
			 *    0 ---- lower ++++++++ upper -- R
			 * If upper < lower the allowed range is:
			 *    0 +++ upper ------ lower +++++ R
			 */
			void set_lower(K lower) {
				lower_ = wrap(lower);
			}

			/** Sets upper value of allowed range
			 * See set_lower for more information
			 */
			void set_upper(K upper) {
				upper_ = wrap(upper);
			}

		private:
			K lower_, upper_;
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
	{
		virtual void print(std::ostream& os) const {
			os << "AngularSpace";
		}
	};

	template<typename State>
	struct FullAngularSpace
	: public AngularSpace<State, Domains::Full<State> >
	{
		virtual void print(std::ostream& os) const {
			os << "FullAngularSpace";
		}
	};

	template<typename State>
	struct IntervalAngularSpace
	: public AngularSpace<State, Domains::Interval<State> >
	{
		virtual void print(std::ostream& os) const {
			os << "IntervalAngularSpace";
		}
	};

}

//---------------------------------------------------------------------------
}}
//---------------------------------------------------------------------------
#endif
