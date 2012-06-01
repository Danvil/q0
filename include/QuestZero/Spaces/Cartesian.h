/*
 * Cartesian.h
 *
 *  Created on: Sep 10, 2010
 *      Author: david
 */

#ifndef SPACE_CARTESIAN_H_
#define SPACE_CARTESIAN_H_
//---------------------------------------------------------------------------
#include "BaseSpace.h"
#include "QuestZero/Common/RandomNumbers.h"
#include "QuestZero/Common/Exceptions.h"
#include <QuestZero/Common/Tools.hpp>
#include <Eigen/Dense>
#include <vector>
#include <cassert>
//---------------------------------------------------------------------------
namespace Q0 {
namespace Spaces {
//---------------------------------------------------------------------------

namespace Cartesian {

	/** A cartesian state vector has to provide the following operations:
	 * - state_t operator*(scalar_t, state_t) for multiplication of a vector by a scalar
	 * - state_t operator+(state_t, state_t) for addition of two vectors
	 * - state_t operator-(state_t) for unary negation of a vectors
	 * - scalar_t operator[](unsigned int i) for access of the i-th element
	 */

	template<typename State>
	struct VectorTraits;

	template<typename K, int N, int _Options, int _MaxRows, int _MaxCols>
	struct VectorTraits<Eigen::Matrix<K,N,1,_Options,_MaxRows,_MaxCols>>
	{
		typedef Eigen::Matrix<K,N,1,_Options,_MaxRows,_MaxCols> state_t;
		typedef K scalar_t;
		static const unsigned int dimension = N;
	};

	namespace Operations
	{
		template<typename State>
		struct Linear
		{
			struct WeightedSumException {};

			typedef typename VectorTraits<State>::scalar_t scalar_t;
			static const unsigned int dim = VectorTraits<State>::dimension;

			double distance(const State& x, const State& y) const {
				State d = difference(x, y);
				scalar_t sum = 0;
				for(unsigned int i=0; i<dim; i++) {
					scalar_t q = d[i];
					sum += q*q;
				}
				return static_cast<double>(std::sqrt(sum));
			}

			State scale(const State& a, double s) const {
				return scalar_t(s) * a;
			}

			State inverse(const State& a) const {
				return -a;
			}

			State compose(const State& a, const State& b) const {
				return a + b;
			}

			State difference(const State& a, const State& b) const {
				return a + (-b);
			}

			template<typename K>
			State weightedSum(K f1, const State& s1, K f2, const State& s2) const {
				return static_cast<scalar_t>(f1) * s1 + static_cast<scalar_t>(f2) * s2;
			}

			template<typename K>
			State weightedSum(K f1, const State& s1, K f2, const State& s2, K f3, const State& s3) const {
				return static_cast<scalar_t>(f1) * s1 + static_cast<scalar_t>(f2) * s2 + static_cast<scalar_t>(f3) * s3;
			}

			template<typename K>
			State weightedSum(const std::vector<K>& factors, const std::vector<State>& states) const {
				INVALID_SIZE_EXCEPTION(factors.size() != states.size()) // Number of factors and states must be equal!
				INVALID_SIZE_EXCEPTION(states.size() == 0)  // Must have at least one element for WeightedSum!
				State c = static_cast<scalar_t>(factors[0]) * states[0];
				for(size_t i=1; i<states.size(); i++) {
					c += static_cast<scalar_t>(factors[i]) * states[i];
				}
				return c;
			}

		protected:
			~Linear() {}
		};
	}

	//---------------------------------------------------------------------------

	namespace Domains
	{
		/** Infinite Cartesian space
		 * Unconstrained random sampling uses a normal distribution around 0 with given sigma
		 */
		template<typename State>
		struct Infinite
		{
			typedef typename VectorTraits<State>::scalar_t S;

			Infinite() : noise_scale_(S(1)), noise_sigma_(S(1)) {}

			/** Sets the normal distribution sigma used in unconstrained random sampling */
			void SetNoiseVariables(S scale, S sigma=S(1)) {
				noise_scale_ = scale;
				noise_sigma_ = sigma;
			}

			// TODO: allow states with variable dimension?
			size_t dimension() const {
				return VectorTraits<State>::dimension;
			}

			State zero() const {
				State v;
				for(size_t i=0; i<dimension(); i++) {
					v[i] = S(0);
				}
				return v;
			}

			State unit(unsigned int k) const {
				State v;
				for(size_t i=0; i<dimension(); i++) {
					v[i] = (i == size_t(k)) ? S(1) : S(0);
				}
				return v;
			}

			template<typename SCL>
			State unit(unsigned int k, SCL s) const {
				State v;
				for(size_t i=0; i<dimension(); i++) {
					v[i] = (i == size_t(k)) ? S(s) : S(0);
				}
				return v;
			}

			const State& project(const State& s) const {
				return s;
			}

			State random() const {
				State v;
				for(size_t i=0; i<dimension(); i++) {
					v[i] = noise_scale_ * RandomNumbers::Normal<S>(noise_sigma_);
				}
				return v;
			}

			template<typename K>
			State random(const State& center, const std::vector<K>& noise) const {
				INVALID_SIZE_EXCEPTION(noise.size() != dimension())
				State v;
				for(size_t i=0; i<dimension(); i++) {
					S n = S(noise[i]);
					S c_min = center[i] - n;
					S c_max = center[i] + n;
					v[i] = RandomNumbers::Uniform(c_min, c_max);
				}
				return v;
			}

		private:
			S noise_scale_;
			S noise_sigma_;

		protected:
			~Infinite() {}
		};

		/** Finite box shaped part of Cartesian space */
		template<typename State>
		struct Box
		{
			typedef typename VectorTraits<State>::scalar_t S;

			Box() {}

			Box(const State& min, const State& max)
			: min_(min),
			  max_(max) {}

			void setDomainRange(const State& range) {
				min_ = -range;
				max_ = range;
			}

			void setDomainMin(const State& min) {
				min_ = min;
			}

			void setDomainMax(const State& max) {
				max_ = max;
			}

			// TODO: allow states with variable dimension?
			size_t dimension() const {
				return VectorTraits<State>::dimension;
			}

			State zero() const {
				State v;
				for(size_t i=0; i<dimension(); i++) {
					v[i] = S(0);
				}
				return v;
			}

			State unit(unsigned int k) const {
				State v;
				for(size_t i=0; i<dimension(); i++) {
					v[i] = (i == size_t(k)) ? S(1) : S(0);
				}
				return v;
			}

			template<typename SCL>
			State unit(unsigned int k, SCL s) const {
				State v;
				for(size_t i=0; i<dimension(); i++) {
					v[i] = (i == size_t(k)) ? S(s) : S(0);
				}
				return v;
			}

			State project(const State& s) const {
				State v;
				for(size_t i=0; i<dimension(); i++) {
					// clamp value to the allowed interval
					v[i] = Clamp(s[i], min_[i], max_[i]);
				}
				return v;
			}

			State random() const {
				State v;
				for(size_t i=0; i<dimension(); i++) {
					// pick uniformly distributed element in the allowed interval
					v[i] = RandomNumbers::Uniform(min_[i], max_[i]);
				}
				return v;
			}

			template<typename K>
			State random(const State& center, const std::vector<K>& noise) const {
				INVALID_SIZE_EXCEPTION(noise.size() != dimension())
				State v;
				for(size_t i=0; i<dimension(); i++) {
					// build the sample interval [c1,c2]
					S n = S(noise[i]);
					assert(n >= S(0)); // noise must be positive!
					S c1 = center[i] - n;
					S c2 = center[i] + n;
					assert(min_[i] < c2 && c1 < max_[i]); // hopefully the intervals overlap
					// compute union of intervals
					S c_min = std::max(min_[i], c1);
					S c_max = std::min(max_[i], c2);
					// pick uniformly distributed element in the interval
					v[i] = RandomNumbers::Uniform(c_min, c_max);
				}
				return v;
			}

		private:
			State min_, max_;

		protected:
			~Box() {}
		};

		/** Finite interval in 1D Cartesian space */
		template<typename State>
		struct Interval
		{
			Interval() {}

			Interval(const State& min, const State& max)
			: min_(min),
			  max_(max) {}

			void setDomainRange(const State& range) {
				min_ = -range;
				max_ = range;
			}

			void setDomainMin(const State& min) {
				min_ = min;
			}

			void setDomainMax(const State& max) {
				max_ = max;
			}

			size_t dimension() const {
				return 1;
			}

			State zero() const {
				return State(0);
			}

			State unit(unsigned int) const {
				return State(1);
			}

			template<typename SCL>
			State unit(unsigned int, SCL s) const {
				return State(s);
			}

			State project(const State& s) const {
				return Clamp(s, min_, max_);
			}

			State random() const {
				return RandomNumbers::Uniform(min_, max_);
			}

			template<typename K>
			State random(const State& center, const std::vector<K>& noise) const {
				INVALID_SIZE_EXCEPTION(noise.size() != dimension())
				State n = State(noise[0]);
				State c_min = std::max(min_, center - n);
				State c_max = std::min(max_, center + n);
				return RandomNumbers::Uniform(c_min, c_max);
			}

		private:
			State min_, max_;

		protected:
			~Interval() {}
		};
	}

	//---------------------------------------------------------------------------

	template<
		typename State,
		class Domain = Domains::Infinite<State>,
		class Operator = Operations::Linear<State>,
		class OperationFinal = OperationFinalPolicy::Projected<State>
	>
	struct CartesianSpace
	: public BaseSpace<State, Operator, Domain, OperationFinal>
	{
		virtual void print(std::ostream& os) const {
			os << "CartesianSpace";
		}
	};

	template<typename State>
	struct InfiniteCartesianSpace
	: public CartesianSpace<State, Domains::Infinite<State>, Operations::Linear<State>, OperationFinalPolicy::Unprojected<State> >
	{
		virtual void print(std::ostream& os) const {
			os << "InfiniteCartesianSpace";
		}
	};

	template<typename State>
	struct FiniteCartesianSpace
	: public CartesianSpace<State, Domains::Box<State> >
	{
		virtual void print(std::ostream& os) const {
			os << "FiniteCartesianSpace";
		}
	};

	template<typename K>
	struct IntervalSpace
	: public CartesianSpace<K, Domains::Interval<K> >
	{
		virtual void print(std::ostream& os) const {
			os << "IntervalSpace";
		}
	};

}

//---------------------------------------------------------------------------
}}
//---------------------------------------------------------------------------
#endif
