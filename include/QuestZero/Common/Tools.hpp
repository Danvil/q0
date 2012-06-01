/*
 * Tools.hpp
 *
 *  Created on: Jun 1, 2012
 *      Author: david
 */

#ifndef TOOLS_HPP_
#define TOOLS_HPP_
//---------------------------------------------------------------------------
#include <cassert>
#include <cmath>
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

/** Clamps x to the inveral [min|max] */
template<typename K>
K Clamp(K x, K min, K max) {
	assert(min <= max);
	return (x < min) ? min : (x > max ? max : x);
}

//---------------------------------------------------------------------------

namespace detail
{
	inline float SignedMod(float a, float b) {
		return std::fmod(a, b);
	}
	inline double SignedMod(double a, double b) {
		return std::fmod(a, b);
	}
	inline long double SignedMod(long double a, long double b) {
		return std::fmod(a, b);
	}
	inline char SignedMod(char a, char b) {
		return a % b;
	}
	inline short SignedMod(short a, short b) {
		return a % b;
	}
	inline int SignedMod(int a, int b) {
		return a % b;
	}
	inline long int SignedMod(long int a, long int b) {
		return a % b;
	}
}

/** Computes 0 <= a < delta s.t. x = a + n * delta for an integer n  */
template<typename K>
K Wrap(K x, K delta) {
	assert(delta > K(0));
	x = detail::SignedMod(x, delta);
	return (x < 0) ? (x + delta) : x;
}

inline unsigned char Wrap(unsigned char x, unsigned char delta) {
	return x % delta;
}

inline unsigned short Wrap(unsigned short x, unsigned short delta) {
	return x % delta;
}

inline unsigned int Wrap(unsigned int x, unsigned int delta) {
	return x % delta;
}

inline unsigned long int Wrap(unsigned long int x, unsigned long int delta) {
	return x % delta;
}

/** Computes min <= a < max s.t.d x = a + n*(max - min)
 * Same as Wrap(x - min, max - min) + min
 */
template<typename K>
K Wrap(K x, K min, K max) {
	assert(min < max);
	return Wrap(x - min, max - min) + min;
}

inline unsigned char Wrap(unsigned char x, unsigned char min, unsigned char max) {
	assert(min < max);
	unsigned char delta = max - min;
	while(x < min) x += delta;
	return x % delta;
}

inline unsigned short Wrap(unsigned short x, unsigned short min, unsigned short max) {
	assert(min < max);
	unsigned short delta = max - min;
	while(x < min) x += delta;
	return x % delta;
}

inline unsigned int Wrap(unsigned int x, unsigned int min, unsigned int max) {
	assert(min < max);
	unsigned int delta = max - min;
	while(x < min) x += delta;
	return x % delta;
}

inline unsigned long int Wrap(unsigned long int x, unsigned long int min, unsigned long int max) {
	assert(min < max);
	unsigned long int delta = max - min;
	while(x < min) x += delta;
	return x % delta;
}

//---------------------------------------------------------------------------

namespace detail
{
	/** Machine epsilon gives an upper bound on the relative error due to rounding in floating point arithmetic.
	 * See http://en.wikipedia.org/wiki/Machine_epsilon
	 */
	template<typename K>
	K Epsilon();

	template<> inline
	float Epsilon<float>() {
		return 5.96e-05f; // machine epsilon: 5.96e-08;
	}

	template<> inline
	double Epsilon<double>() {
		return 1.11e-12; // machine epsilon: 1.11e-16;
	}

	template<> inline
	long double Epsilon<long double>() {
		return 9.63e-30; // machine epsilon: 9.63e-35;
	}
}

template<typename K>
bool IsEqualEpsilon(K a, K b, K epsilon=detail::Epsilon<K>()) {
	return std::abs(a - b) < epsilon;
}

template<typename K>
bool IsZeroEpsilon(K a, K epsilon=detail::Epsilon<K>()) {
	return IsEqualEpsilon(a, K(0), epsilon);
}

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
