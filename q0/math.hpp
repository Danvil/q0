#ifndef Q0_MATH_HPP_
#define Q0_MATH_HPP_
#include <q0/common.hpp>
#include <boost/random.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/normal_distribution.hpp>
#include <vector>
#include <algorithm>
//---------------------------------------------------------------------------
namespace q0 { namespace math {

namespace detail {
	inline boost::random::mt19937& random_engine() {
		static boost::random::mt19937 rng;
		return rng;
	}
}

/** Returns a uniformly distributed number from the interval [a,b] */
template<typename K>
K random_uniform(K a, K b) {
	boost::random::uniform_real_distribution<K> dist(a,b);
	return dist(detail::random_engine());
}

/** Returns a uniformly distributed number with mean 0 and standard deviation 1 */
template<typename K>
K random_stddev() {
	static boost::random::normal_distribution<K> dist;
	return dist(detail::random_engine());
}

/** Returns b which solves x = n*a + b, 0 <= b < a, n integer */
template<typename K>
K wrap(K x, K a) {
	// FIXME faster!
	while(x >= a) x -= a;
	while(x < a) x += a;
	return x;
}

/** Returns max(a, min(b, x)) */
template<typename K>
K clamp(K x, K a, K b) {
	return std::max(a, std::min(b, x));
}

}}
//---------------------------------------------------------------------------
#endif
