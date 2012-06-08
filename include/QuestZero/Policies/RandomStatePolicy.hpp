/*
 * RandomStatePolicy.hpp
 *
 *  Created on: Jun 8, 2012
 *      Author: david
 */

#ifndef RANDOMSTATEPOLICY_HPP_
#define RANDOMSTATEPOLICY_HPP_

#include <QuestZero/Common/VectorTraits.hpp>
#include <QuestZero/Common/RandomNumbers.h>
#include <functional>

namespace Q0
{

	/** Picks random Cartesian states using a normal distribution */
	template<typename State>
	struct NormalRandomCartesianStatePolicy
	{
		typedef VectorTraits<State> traits_t;
		typedef typename traits_t::scalar_t scalar_t;

		/** Sets mean and standard deviation for randomly picking normal distributed states */
		void SetRandomStateNormalDistribution(const State& mean, const State& stddev) {
			rnd_mean_ = mean;
			rnd_stddev_ = stddev;
		}

		/** Picks a random state */
		State random() const {
			State v = rnd_mean_;
			for(size_t i=0; i<traits_t::dim(v); i++) {
				traits_t::at(v, i) += RandomNumbers::Normal<scalar_t>(traits_t::at(rnd_stddev_,i));
			}
			return v;
		}

	protected:
		// policies have virtual destructor
		~NormalRandomCartesianStatePolicy() {}

	private:
		State rnd_mean_;
		State rnd_stddev_;
	};

	/** Picks random Cartesian states uniformly inside a box */
	template<typename State>
	struct UniformRandomCartesianStatePolicy
	{
		typedef VectorTraits<State> traits_t;
		typedef typename traits_t::scalar_t scalar_t;

		/** Sets the box in which random states are picked uniformly using min and max coordinates */
		void SetRandomStateRange(const State& min, const State& max) {
			rnd_min_ = min;
			rnd_max_ = max;
		}

		/** Picks a random state */
		State random() const {
			State v = traits_t::create(traits_t::dim(rnd_min_));
			for(size_t i=0; i<traits_t::dim(v); i++) {
				scalar_t c_min = traits_t::at(rnd_min_, i);
				scalar_t c_max = traits_t::at(rnd_max_, i);
				traits_t::at(v, i) = RandomNumbers::Uniform(c_min, c_max);
			}
			return v;
		}

	protected:
		// policies have virtual destructor
		~UniformRandomCartesianStatePolicy() {}

	private:
		State rnd_min_;
		State rnd_max_;
	};

	/** Picks random Cartesian states using a functor */
	template<typename State>
	struct FunctorRandomCartesianStatePolicy
	{
		typedef VectorTraits<State> traits_t;
		typedef typename traits_t::scalar_t scalar_t;
		typedef std::function<State()> rnd_functor_t;

		/** Sets the functor used to pick random states */
		void SetRandomFunctor(const rnd_functor_t& f) {
			rnd_functor_ = f;
		}

		/** Picks a random state */
		State random() const {
			return rnd_functor_();
		}

	protected:
		// policies have virtual destructor
		~FunctorRandomCartesianStatePolicy() {}

	private:
		rnd_functor_t rnd_functor_;
	};
}

#endif
