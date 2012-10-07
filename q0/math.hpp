#ifndef Q0_MATH_HPP_
#define Q0_MATH_HPP_
#include <q0/common.hpp>
#include <boost/random.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/assert.hpp>
#include <vector>
#include <algorithm>
#include <type_traits>
//---------------------------------------------------------------------------
namespace q0 { namespace math {

namespace detail {
	inline boost::random::mt19937& random_engine() {
		static boost::random::mt19937 rng;
		return rng;
	}
}

/** Returns a uniformly distributed random integer in {a,...,b} */
template<typename K>
K random_int(K a, K b) {
	static_assert(std::is_integral<K>::value, "random_int requires integral type!");
	boost::random::uniform_int_distribution<K> dist(a, b);
	return dist(detail::random_engine());
}

/** Returns a uniformly distributed number from the interval [a,b] */
template<typename K>
K random_uniform(K a, K b) {
	static_assert(std::is_floating_point<K>::value, "random_uniform requires floating point type!");
	boost::random::uniform_real_distribution<K> dist(a, b);
	return dist(detail::random_engine());
}

/** Returns a uniformly distributed number with mean 0 and standard deviation 1 */
template<typename K>
K random_stddev() {
	static_assert(std::is_floating_point<K>::value, "random_stddev requires floating point type!");
	static boost::random::normal_distribution<K> dist;
	return dist(detail::random_engine());
}

/** Returns b which solves x = n*a + b, 0 <= b < a, n integer */
template<typename K>
K wrap(K x, K a) {
	BOOST_ASSERT(0 < a);
	// FIXME faster!
	while(x < 0) x += a;
	while(x >= a) x -= a;
	return x;
}

/** Returns a + wrap(x-a, b-a) */
template<typename K>
K wrap(K x, K a, K b) {
	BOOST_ASSERT(b > a);
	return a + wrap(x-a, b-a);
}

template<typename K>
K wrap_2pi(K x) {
	return wrap(x, K(2)*boost::math::constants::pi<K>());
}

/** Returns max(a, min(b, x)) */
template<typename K>
K clamp(K x, K a, K b) {
	return std::max(a, std::min(b, x));
}

}}
//---------------------------------------------------------------------------
#endif
