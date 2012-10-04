#ifndef Q0_MATH_HPP_
#define Q0_MATH_HPP_
#include <q0/common.hpp>
#include <boost/random.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <vector>
#include <algorithm>
//---------------------------------------------------------------------------
namespace q0 { namespace math {

/** Returns b which solves x = n*a + b, 0 <= b < a, n integer */
template<typename K>
K wrap(K x, K a) {
	// FIXME faster!
	while(x >= a) x -= a;
	while(x < a) x += a;
	return x;
}

}}
//---------------------------------------------------------------------------
#endif
