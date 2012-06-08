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

	template<typename State>
	struct NormalRandomCartesianStatePolicy
	{
		typedef VectorTraits<State> traits_t;
		typedef typename traits_t::scalar_t scalar_t;

		State random() const {
			State v = rnd_mean_;
			for(size_t i=0; i<traits_t::dim(v); i++) {
				traits_t::at(v, i) += RandomNumbers::Normal<scalar_t>(traits_t::at(rnd_stddev_,i));
			}
			return v;
		}

		void SetRandomStateNormalDistribution(const State& center, const State& stddev) {
			rnd_mean_ = center;
			rnd_stddev_ = stddev;
		}

	protected:
		// policies have virtual destructor
		~NormalRandomCartesianStatePolicy() {}

	private:
		State rnd_mean_;
		State rnd_stddev_;
	};

	template<typename State>
	struct UniformRandomCartesianStatePolicy
	{
		typedef VectorTraits<State> traits_t;
		typedef typename traits_t::scalar_t scalar_t;

		State random() const {
			State v = traits_t::create(traits_t::dim(rnd_min_));
			for(size_t i=0; i<traits_t::dim(v); i++) {
				scalar_t c_min = traits_t::at(rnd_min_, i);
				scalar_t c_max = traits_t::at(rnd_max_, i);
				traits_t::at(v, i) = RandomNumbers::Uniform(c_min, c_max);
			}
			return v;
		}

		void SetRandomStateRange(const State& min, const State& max) {
			rnd_min_ = min;
			rnd_max_ = max;
		}

	protected:
		// policies have virtual destructor
		~UniformRandomCartesianStatePolicy() {}

	private:
		State rnd_min_;
		State rnd_max_;
	};

	template<typename State>
	struct FunctorRandomCartesianStatePolicy
	{
		typedef VectorTraits<State> traits_t;
		typedef typename traits_t::scalar_t scalar_t;
		typedef std::function<State()> rnd_functor_t;

		void SetRandomFunctor(const rnd_functor_t& f) {
			rnd_functor_ = f;
		}

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
