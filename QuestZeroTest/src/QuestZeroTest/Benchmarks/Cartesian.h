/*
 * Cartesian.h
 *
 *  Created on: Sep 5, 2010
 *      Author: david
 */

#ifndef QUESTZERO_BENCHMARKS_CARTESIAN_H
#define QUESTZERO_BENCHMARKS_CARTESIAN_H

#include <Danvil/Tools/Constants.h>
#include <Danvil/LinAlg.h>
#include <cmath>

namespace Benchmarks {

template<typename K, int N>
class Cartesian
{
public:
	typedef Danvil::ctLinAlg::Vec<K, N> V;

public:
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
		for(unsigned int i = 0; i < v.dimension(); i++) {
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
		for(unsigned int i = 0; i < v.dimension(); i++) {
			K x = abs(v[i]);
			if(x > max) {
				max = x;
			}
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
		for(unsigned int i = 0; i < v.dimension(); i++) {
			K x = v[i];
			if(x > max) {
				max = x;
			}
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
		for(unsigned int i = 0; i < v.dimension(); i++) {
			K x = abs(v[i]);
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
		for(unsigned int i = 0; i < v.dimension(); i++) {
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
		for(int i = 0; i < v.dimension() - 1; i++) {
			K x = v[i];
			K y = v[i + 1];
			K a = y - x * x;
			K b = x - 1;
			sum += 100 * a * a + b * b;
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
		K sum = 0;
		for(unsigned int i = 0; i < v.dimension(); i++) {
			K x = v[i];
			sum += x * x + 10 * (1 - cos(Danvil::C_2_PI * x));
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
		for(unsigned int i = 0; i < v.dimension(); i++) {
			K a = floor(v[i] + 0.5);
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
		for(unsigned int i = 0; i < v.dimension(); i++) {
			K x = v[i];
			sum += x * x;
			prod *= cos(Danvil::C_2_PI * x);
		}
		return Danvil::C_EULER + 20 - 20 * exp(-0.2 * sqrt(sum / v.dimension())) - exp(prod / v.dimension());
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
		for(unsigned int i = 0; i < v.dimension(); i++) {
			K x = v[i];
			sum += x * x;
			prod *= cos(x / sqrt(i + 1));
		}
		return 1 + sum / 4000 - prod;
	}

};

}

#endif