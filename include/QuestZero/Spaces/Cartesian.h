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
#include <QuestZero/Common/VectorTraits.hpp>
#include <QuestZero/Policies/RandomStatePolicy.hpp>
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

	namespace Operations
	{
		template<typename State>
		struct Linear
		{
			struct WeightedSumException {};

			typedef VectorTraits<State> traits_t;
			typedef typename traits_t::scalar_t scalar_t;

			double distance(const State& x, const State& y) const {
				State d = difference(x, y);
				scalar_t sum = 0;
				for(unsigned int i=0; i<traits_t::dim(d); i++) {
					scalar_t q = traits_t::at(d, i);
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
		template<typename State, template<typename> class RandomStatePolicy>
		struct Infinite
		: public RandomStatePolicy<State>
		{
			typedef VectorTraits<State> traits_t;
			typedef typename traits_t::scalar_t S;

			Infinite(unsigned int dim=traits_t::dimension)
			: dimension_(dim),
			  noise_scale_(S(1)),
			  noise_sigma_(S(1))
			{}

			void setDimension(unsigned int dim) {
				if(traits_t::dimension > 0) {
					assert(dim == traits_t::dimension);
				}
				else {
					dimension_ = dim;
				}
			}

			/** Sets the normal distribution sigma used in unconstrained random sampling */
			void SetNoiseVariables(S scale, S sigma=S(1)) {
				noise_scale_ = scale;
				noise_sigma_ = sigma;
			}

			// TODO: allow states with variable dimension?
			size_t dimension() const {
				return dimension_;
			}

			State zero() const {
				State v = traits_t::create(dimension_);
				for(size_t i=0; i<dimension(); i++) {
					traits_t::at(v, i) = S(0);
				}
				return v;
			}

			State unit(unsigned int k) const {
				State v = traits_t::create(dimension_);
				for(size_t i=0; i<dimension(); i++) {
					traits_t::at(v, i) = (i == size_t(k)) ? S(1) : S(0);
				}
				return v;
			}

			template<typename SCL>
			State unit(unsigned int k, SCL s) const {
				State v = traits_t::create(dimension_);
				for(size_t i=0; i<dimension(); i++) {
					traits_t::at(v, i) = (i == size_t(k)) ? S(s) : S(0);
				}
				return v;
			}

			const State& project(const State& s) const {
				return s;
			}

			State random() const {
				return RandomStatePolicy<State>::random();
			}

			template<typename K>
			State random(const State& center, const std::vector<K>& noise) const {
				INVALID_SIZE_EXCEPTION(noise.size() != dimension())
				State v = traits_t::create(dimension_);
				for(size_t i=0; i<dimension(); i++) {
					S n = S(noise[i]);
					S c_min = traits_t::at(center, i) - n;
					S c_max = traits_t::at(center, i) + n;
					traits_t::at(v, i) = RandomNumbers::Uniform(c_min, c_max);
				}
				return v;
			}

		private:
			unsigned int dimension_;
			S noise_scale_;
			S noise_sigma_;

		protected:
			~Infinite() {}
		};

		/** Finite box shaped part of Cartesian space */
		template<typename State>
		struct Box
		{
			typedef VectorTraits<State> traits_t;
			typedef typename traits_t::scalar_t S;

			Box() {}

			Box(const State& min, const State& max) {
				setMinMax(min, max);
			}

			void setMinMax(const State& min, const State& max) {
				assert(traits_t::dim(min) == traits_t::dim(max));
				min_ = min;
				max_ = max;
			}

			void setDomainRange(const State& range) {
				setMinMax(-range, range);
			}

			void setDomainMin(const State& min) {
				min_ = min;
			}

			void setDomainMax(const State& max) {
				max_ = max;
			}

			// TODO: allow states with variable dimension?
			size_t dimension() const {
				return traits_t::dim(min_);
			}

			State zero() const {
				State v = traits_t::create(dimension());
				for(size_t i=0; i<dimension(); i++) {
					traits_t::at(v, i) = S(0);
				}
				return v;
			}

			State unit(unsigned int k) const {
				State v = traits_t::create(dimension());
				for(size_t i=0; i<dimension(); i++) {
					traits_t::at(v, i) = (i == size_t(k)) ? S(1) : S(0);
				}
				return v;
			}

			template<typename SCL>
			State unit(unsigned int k, SCL s) const {
				State v = traits_t::create(dimension());
				for(size_t i=0; i<dimension(); i++) {
					traits_t::at(v, i) = (i == size_t(k)) ? S(s) : S(0);
				}
				return v;
			}

			State project(const State& s) const {
				State v = traits_t::create(dimension());
				for(size_t i=0; i<dimension(); i++) {
					// clamp value to the allowed interval
					traits_t::at(v, i) = Clamp(traits_t::at(s, i), traits_t::at(min_, i), traits_t::at(max_, i));
				}
				return v;
			}

			State random() const {
				State v = traits_t::create(dimension());
				for(size_t i=0; i<dimension(); i++) {
					// pick uniformly distributed element in the allowed interval
					traits_t::at(v, i) = RandomNumbers::Uniform(traits_t::at(min_, i), traits_t::at(max_, i));
				}
				return v;
			}

			template<typename K>
			State random(const State& center, const std::vector<K>& noise) const {
				INVALID_SIZE_EXCEPTION(noise.size() != dimension())
				State v = traits_t::create(dimension());
				for(size_t i=0; i<dimension(); i++) {
					// build the sample interval [c1,c2]
					S n = S(traits_t::at(noise, i));
					assert(n >= S(0)); // noise must be positive!
					S c1 = traits_t::at(center, i) - n;
					S c2 = traits_t::at(center, i) + n;
					assert(traits_t::at(min_, i) < c2 && c1 < traits_t::at(max_, i)); // hopefully the intervals overlap
					// compute union of intervals
					S c_min = std::max(traits_t::at(min_, i), c1);
					S c_max = std::min(traits_t::at(max_, i), c2);
					// pick uniformly distributed element in the interval
					traits_t::at(v, i) = RandomNumbers::Uniform(c_min, c_max);
				}
				return v;
			}

		private:
			State min_, max_;

		protected:
			~Box() {}
		};
	}

	template<
		typename State,
		typename Domain,
		typename Operator = Cartesian::Operations::Linear<State>,
		typename OperationFinal = OperationFinalPolicy::Projected<State>
	>
	struct CartesianSpace
	: public BaseSpace<State, Operator, Domain, OperationFinal>
	{
		virtual void print(std::ostream& os) const {
			os << "CartesianSpace";
		}
	};

}

//---------------------------------------------------------------------------

template<typename State, template<typename> class RandomStatePolicy = NormalRandomCartesianStatePolicy>
struct InfiniteCartesianSpace
: public Cartesian::CartesianSpace<
  	  State,
  	  Cartesian::Domains::Infinite<State,RandomStatePolicy>,
  	  Cartesian::Operations::Linear<State>,
  	  OperationFinalPolicy::Unprojected<State> >
{
	virtual void print(std::ostream& os) const {
		os << "InfiniteCartesianSpace";
	}
};

template<typename State>
struct FiniteCartesianSpace
: public Cartesian::CartesianSpace<
  	  State,
  	  Cartesian::Domains::Box<State> >
{
	virtual void print(std::ostream& os) const {
		os << "FiniteCartesianSpace";
	}
};

template<typename K>
struct IntervalSpace
: public Cartesian::CartesianSpace<
  	  K,
  	  Cartesian::Domains::Box<K> >
{
	virtual void print(std::ostream& os) const {
		os << "IntervalSpace";
	}
};

//---------------------------------------------------------------------------
}}
//---------------------------------------------------------------------------
#endif
