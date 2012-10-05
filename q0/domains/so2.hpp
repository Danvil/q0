#ifndef Q0_DOMAINS_SO2_H_
#define Q0_DOMAINS_SO2_H_
#include <q0/common.hpp>
#include <q0/math.hpp>
#include <q0/domains.hpp>
#include <boost/math/constants/constants.hpp>
#include <vector>
//---------------------------------------------------------------------------
namespace q0 { namespace domains {

template<typename K>
struct so2 {};

template<typename K>
struct tangent_size<so2<K>> : std::integral_constant<int,1> {};

template<typename K>
struct angle {
	K value;
	operator K() const {
		return value;
	}
	K rad() const {
		return value;
	}
	K deg() const {
		return (K(180) * value) / boost::math::constants::pi<K>();
	}
	std::complex<K> circle_point() const {
		return std::complex<K>(std::cos(value), std::sin(value));
	}
};

template<typename K>
struct state_type<so2<K>> {
	// the angle is used because SO(2) is isomorphic to R/2piZ
	typedef angle<K> type;
};

template<typename T, typename K>
typename state_type<so2<K>>::type exp(const so2<K>&, const typename state_type<so2<K>>::type&, const typename tangent_type<T,so2<K>>::type& y) {
	// limiting the angle is imperative!
	return {math::wrap(y, 2*boost::math::constants::pi<K>())};
}

template<typename T, typename K>
typename tangent_type<T,so2<K>>::type log(const so2<K>&, const typename state_type<so2<K>>::type&, const typename state_type<so2<K>>::type& y) {
	return y;
}

template<typename K>
typename state_type<so2<K>>::type random(const so2<K>& dom) {
	return {math::random_uniform<K>(0, 2*boost::math::constants::pi<K>())};
}

template<typename K>
typename state_type<so2<K>>::type random_neighbour(const so2<K>&, const typename state_type<so2<K>>::type& x, double radius) {
	if(radius > boost::math::constants::pi<double>()) {
		radius = boost::math::constants::pi<double>();
	}
	//return {math::wrap(x + math::random<K>(-radius, +radius), 2*boost::math::constants::pi<K>())};
	return {math::wrap<K>(x + radius*math::random_stddev<K>(), 2*boost::math::constants::pi<K>())};
}

template<typename W, typename K>
typename state_type<so2<K>>::type mean(const so2<K>&, const std::vector<W>& weights, const std::vector<typename state_type<so2<K>>::type>& states) {
	BOOST_ASSERT(states.size() == weights.size());
	BOOST_ASSERT(states.size() > 0);
	K weight_sum = 0;
	K x = 0;
	const std::size_t n = weights.size();
	for(std::size_t i=0; i<n; i++) {
		K w = static_cast<K>(weights[i]);
		BOOST_ASSERT(w >= 0);
		x += w*states[i];
		weight_sum += w;
	}
	BOOST_ASSERT(weight_sum >= 0);
	x *= (K(1) / weight_sum);
	return {x};
}

}}
//---------------------------------------------------------------------------
#endif
