#ifndef Q0_DOMAINS_CARTESIAN_H_
#define Q0_DOMAINS_CARTESIAN_H_
#include <q0/common.hpp>
#include <q0/tools.hpp>
#include <q0/domains.hpp>
#include <vector>
//---------------------------------------------------------------------------
namespace q0 { namespace domains {

template<typename K, unsigned int N>
struct cartesian
{
	K random_range;

	cartesian() : random_range(K(1000)) {}

};

template<typename K, unsigned int N>
struct state_type<cartesian<K,N>> {
	typedef typename vec<K,N>::type type;
};

template<typename K, unsigned int N>
struct tangent_type<cartesian<K,N>> {
	typedef typename vec<K,N>::type type;
};

template<typename K, unsigned int N>
struct scalar_type<cartesian<K,N>> {
	typedef K type;
};

template<typename K, unsigned int N>
unsigned int dimension(const cartesian<K,N>&) {
	return N;
}

template<typename K, unsigned int N>
typename state_type<cartesian<K,N>>::type exp(const cartesian<K,N>&, const typename state_type<cartesian<K,N>>::type& x, const typename tangent_type<cartesian<K,N>>::type& y) {
	return x + y;
}

template<typename K, unsigned int N>
typename tangent_type<cartesian<K,N>>::type log(const cartesian<K,N>&, const typename state_type<cartesian<K,N>>::type& x, const typename state_type<cartesian<K,N>>::type& y) {
	return y - x;
}

template<typename K, unsigned int N>
typename state_type<cartesian<K,N>>::type random(const cartesian<K,N>& dom) {
	typename state_type<cartesian<K,N>>::type x;
	for(unsigned int i=0; i<N; i++) {
		at(x, i) = tools::random<K>(-dom.random_range, dom.random_range);
	}
	return x;
}

template<typename K, unsigned int N>
typename state_type<cartesian<K,N>>::type random_neighbour(const cartesian<K,N>&, const typename state_type<cartesian<K,N>>::type& x, K radius) {
	typename state_type<cartesian<K,N>>::type y = x;
	for(unsigned int i=0; i<N; i++) {
		at(y, i) += tools::random<K>(-radius, +radius);
	}
	return y;
}

template<typename K, unsigned int N>
typename state_type<cartesian<K,N>>::type mean(const cartesian<K,N>&, const std::vector<K>& weights, const std::vector<typename state_type<cartesian<K,N>>::type>& states) {
	const std::size_t n = weights.size();
	if(n != states.size()) {
		// FIXME error
	}
	if(n == 0) {
		// FIXME error
	}
	typename state_type<cartesian<K,N>>::type x = weights[0]*states[0];
	for(std::size_t i=1; i<n; i++) {
		x += weights[i]*states[i];
	}
	return (K(1) / static_cast<K>(n)) * x;
}

}}
//---------------------------------------------------------------------------
#endif
