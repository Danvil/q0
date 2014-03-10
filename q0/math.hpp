#ifndef Q0_MATH_HPP_
#define Q0_MATH_HPP_
#include <random>
#include <boost/math/constants/constants.hpp>
#include <boost/assert.hpp>
#include <type_traits>
//---------------------------------------------------------------------------
namespace q0 { namespace math {

namespace detail {
	inline std::mt19937& random_engine() {
		static std::mt19937 rng;
		return rng;
	}
}

void random_seed(unsigned int i) {
	detail::random_engine().seed(i);
}

/** Returns a uniformly distributed random integer in {a,...,b} */
template<typename K>
K random_int(K a, K b) {
	static_assert(std::is_integral<K>::value, "random_int requires integral type!");
	std::uniform_int_distribution<K> dist(a, b);
	return dist(detail::random_engine());
}

/** Returns a uniformly distributed number from the interval [a,b] */
template<typename K>
K random_uniform(K a, K b) {
	static_assert(std::is_floating_point<K>::value, "random_uniform requires floating point type!");
	std::uniform_real_distribution<K> dist(a, b);
	return dist(detail::random_engine());
}

/** Returns a uniformly distributed number with mean 0 and standard deviation 1 */
template<typename K>
K random_stddev() {
	static_assert(std::is_floating_point<K>::value, "random_stddev requires floating point type!");
	static std::normal_distribution<K> dist;
	return dist(detail::random_engine());
}

/** Returns b which solves x = n*a + b, 0 <= b < a, n integer */
template<typename K>
K wrap(K x, K w) {
	BOOST_ASSERT(w > 0);
	// FIXME faster!
	while(x < 0) x += w;
	while(x >= w) x -= w;
	return x;
}

/** Returns a + wrap(x-a, b-a) */
template<typename K>
K wrap(K x, K a, K b) {
	BOOST_ASSERT(a < b);
	return a + wrap(x-a, b-a);
}

template<typename K>
K wrap_2pi(K x) {
	return wrap(x, K(2)*boost::math::constants::pi<K>());
}

/** Returns max(a, min(b, x)) */
template<typename K>
K clamp(K x, K a, K b) {
	BOOST_ASSERT(a <= b);
	if(x <= a) return a;
	if(x >= b) return b;
	return x;
}

}}
//---------------------------------------------------------------------------
#endif
