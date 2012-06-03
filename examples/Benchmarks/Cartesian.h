/*
 * Cartesian.h
 *
 *  Created on: Sep 5, 2010
 *      Author: david
 */

#ifndef QUESTZERO_BENCHMARKS_CARTESIAN_H
#define QUESTZERO_BENCHMARKS_CARTESIAN_H

#include <QuestZero/Spaces/Cartesian.h>
#include <boost/math/constants/constants.hpp>
#include <cmath>

namespace Benchmarks {

template<typename V>
struct Cartesian
{
	typedef Q0::Spaces::Cartesian::VectorTraits<V> traits_t;
	typedef typename traits_t::scalar_t K;

	/// <summary>
	/// The function f(x) = 0
	/// </summary>
	/// <param name="v"></param>
	/// <returns></returns>
	static K Null(const V& v) {
		return 0;
	}

	/// <summary>
	/// For n=2: The paraboloid function with a unique local/global minimum at (0,0)
	/// http://www.wolframalpha.com/input/?i=x^2+%2B+y^2
	/// </summary>
	/// <param name="v"></param>
	/// <returns></returns>
	static K Sphere(const V& v) {
		K sum = 0;
		for(unsigned int i = 0; i < traits_t::dim(v); i++) {
			K x = v[i];
			sum += x * x;
		}
		return sum;
	}

	/// <summary>
	/// For n=2: A function with a global minimum at (0,0) resembling a turned four sided pyramid
	/// http://www.wolframalpha.com/input/?i=maximum%28abs%28x%29%2C+abs%28y%29%29
	/// </summary>
	/// <param name="v"></param>
	/// <returns></returns>
	static K DiscreetSphere(const V& v) {
		K max = 0;
		for(unsigned int i = 0; i < traits_t::dim(v); i++) {
			max = std::max(max, std::abs(v[i]));
		}
		return max;
	}

	/// <summary>
	/// For n=2: A function with a global minimum in infinity but no local minimum
	/// http://www.wolframalpha.com/input/?i=max%28x%2C+y%29
	/// </summary>
	/// <param name="v"></param>
	/// <returns></returns>
	static K Schwefel2_21(const V& v) {
		K max = 0;
		for(unsigned int i = 0; i < traits_t::dim(v); i++) {
			max = std::max(max, v[i]);
		}
		return max;
	}

	/// <summary>
	/// For n=2: A function with one local/global minimum and a cross shaped section with low function values
	/// http://www.wolframalpha.com/input/?i=abs%28x%29+%2B+abs%28y%29+%2B+abs%28x*y%29
	/// </summary>
	/// <param name="v"></param>
	/// <returns></returns>
	static K Schwefel2_22(const V& v) {
		K sum = 0, prod = 1;
		for(unsigned int i = 0; i < traits_t::dim(v); i++) {
			K x = std::abs(v[i]);
			sum += x;
			prod *= x;
		}
		return sum + prod;
	}

	/// <summary>
	/// For n=2: A function with a global minimum in infinity but no local minimum
	/// http://www.wolframalpha.com/input/?i=x%2Bx%2By
	/// </summary>
	/// <param name="v"></param>
	/// <returns></returns>
	static K Schwefel1_2(const V& v) {
		K sum = 0;
		for(unsigned int i = 0; i < traits_t::dim(v); i++) {
			for(int j = i; j < i; j++) {
				sum += v[i];
			}
		}
		return sum;
	}

	/// <summary>
	/// For n=2: A function with a very flat minimum at (1,1)
	/// http://www.wolframalpha.com/input/?i=100+%28y-x^2%29^2+%2B+%28x-1%29^2
	/// </summary>
	/// <param name="v"></param>
	/// <returns></returns>
	static K Rosenbrock(const V& v) {
		K sum = 0;
		for(unsigned int i = 0; i < traits_t::dim(v) - 1; i++) {
			K x = v[i];
			K y = v[i + 1];
			K a = y - x * x;
			K b = x - 1;
			sum += K(100) * a * a + b * b;
		}
		return sum;
	}

	/// <summary>
	/// For n=2: A function with lots of local minima and one global at (0,0)
	/// http://www.wolframalpha.com/input/?i=x^2+%2B+10+-+10+Cos%282+Pi+x%29+%2B+y^2+%2B+10+-+10+Cos%282+Pi+y%29
	/// </summary>
	/// <param name="v"></param>
	/// <returns></returns>
	static K Rastrigin(const V& v) {
		const K cA = (K)10;
		const K cTwoPi = 2 * boost::math::constants::pi<K>();
		K sum = 0;
		for(unsigned int i = 0; i < traits_t::dim(v); i++) {
			K x = v[i];
			sum += x * x + cA * (1 - std::cos(cTwoPi * x));
		}
		return sum;
	}

	/// <summary>
	/// Like the Sphere function but with discrete steps
	/// http://www.wolframalpha.com/input/?i=floor%28x+%2B+0.5%29^2+%2B+floor%28y+%2B+0.5%29^2
	/// </summary>
	/// <param name="v"></param>
	/// <returns></returns>
	static K Step(const V& v) {
		K sum = 0;
		for(unsigned int i = 0; i < traits_t::dim(v); i++) {
			K a = std::floor(v[i] + K(0.5));
			sum += a * a;
		}
		return sum;
	}

	/// <summary>
	/// For n=2: Noisy function with one global minimum at (0,0)
	/// http://www.wolframalpha.com/input/?i=e+%2B+20+-+20+exp%28-0.2+sqrt%28%28x^2%2By^2%29%2F2%29%29+-+exp%281%2F2+cos%282+pi+x%29+cos%282+pi+y%29%29
	/// </summary>
	/// <param name="v"></param>
	/// <returns></returns>
	static K Ackley(const V& v) {
		K sum = 0, prod = 1;
		for(unsigned int i = 0; i < traits_t::dim(v); i++) {
			K x = v[i];
			sum += x * x;
			prod *= std::cos(K(2) * boost::math::constants::pi<K>() * x);
		}
		return boost::math::constants::euler<K>() + K(20) - K(20) * std::exp(-K(0.2) * std::sqrt(sum / K(traits_t::dim(v)))) - std::exp(prod / K(traits_t::dim(v)));
	}

	/// <summary>
	/// For n=2: This function is the sum of a very flat sphere function plus a "cosine wave field", minimum at (0,0)
	/// http://www.wolframalpha.com/input/?i=1+%2B+1%2F4000+%28x^2+%2B+y^2%29+-+cos%28x%29+cos%28y%2Fsqrt%282%29%29
	/// </summary>
	/// <param name="v"></param>
	/// <returns></returns>
	static K Griewank(const V& v) {
		K sum = 0;
		K prod = 1;
		for(unsigned int i = 0; i < traits_t::dim(v); i++) {
			K x = v[i];
			sum += x * x;
			prod *= std::cos(x / std::sqrt(i + 1));
		}
		return 1 + sum / K(4000) - prod;
	}

};

}

#endif
