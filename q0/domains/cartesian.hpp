#ifndef Q0_DOMAINS_CARTESIAN_H_
#define Q0_DOMAINS_CARTESIAN_H_
#include <q0/common.hpp>
#include <q0/domains.hpp>
#include <q0/math.hpp>
#include <boost/assert.hpp>
#include <vector>
//---------------------------------------------------------------------------
namespace q0 { namespace domains {

template<typename K, unsigned int N>
struct cartesian_base
{
	typedef typename vec<K,N>::type State;

};

//---------------------------------------------------------------------------

template<typename K, unsigned int N>
struct cartesian_constraint_none
{
	K random_std_dev;
	cartesian_constraint_none() : random_std_dev(K(10)) {}
};

template<typename K, unsigned int N>
typename cartesian_base<K,N>::State restrict(const cartesian_constraint_none<K,N>&, const typename cartesian_base<K,N>::State& x) {
	return x;
}

template<typename K, unsigned int N>
typename cartesian_base<K,N>::State random(const cartesian_constraint_none<K,N>& dom) {
	typename cartesian_base<K,N>::State x;
	for(unsigned int i=0; i<N; i++) {
//		at(x, i) = math::random_uniform<K>(-dom.random_range, dom.random_range);
		at(x, i) = dom.random_std_dev*math::random_stddev<K>();
	}
	return x;
}

template<typename W, typename K, unsigned int N>
typename cartesian_base<K,N>::State random_neighbour(const cartesian_constraint_none<K,N>&, const typename cartesian_base<K,N>::State& x, W radius) {
	typename cartesian_base<K,N>::State y;
	K r = static_cast<K>(radius);
	for(unsigned int i=0; i<N; i++) {
//		at(y, i) = at(x, i) + math::random_uniform<K>(-r, +r);
		at(y, i) = at(x, i) + r*math::random_stddev<K>();
	}
	return y;
}

//---------------------------------------------------------------------------

template<typename K, unsigned int N>
struct cartesian_constraint_box
{
	typename vec<K,N>::type min, max;

	cartesian_constraint_box() {
		set_box_extends(-1,+1);
	}

	void set_box_extends(K a, K b) {
		for(unsigned int i=0; i<N; i++) {
			at(min,i) = a;
			at(max,i) = b;
		}
	}
};

template<typename K, unsigned int N>
typename cartesian_base<K,N>::State restrict(const cartesian_constraint_box<K,N>& dom, const typename cartesian_base<K,N>::State& x) {
	typename cartesian_base<K,N>::State y;
	for(unsigned int i=0; i<N; i++) {
		at(y,i) = math::clamp(at(x,i), at(dom.min,i), at(dom.max,i));
	}
	return y;
}

template<typename K, unsigned int N>
typename cartesian_base<K,N>::State random(const cartesian_constraint_box<K,N>& dom) {
	typename cartesian_base<K,N>::State y;
	for(unsigned int i=0; i<N; i++) {
		at(y,i) = math::random_uniform<K>(at(dom.min,i), at(dom.max,i));
	}
	return y;
}

template<typename W, typename K, unsigned int N>
typename cartesian_base<K,N>::State random_neighbour(const cartesian_constraint_box<K,N>& dom, const typename cartesian_base<K,N>::State& x, W radius) {
	typename cartesian_base<K,N>::State y;
	K r = static_cast<K>(radius);
	for(unsigned int i=0; i<N; i++) {
		at(y, i) = at(x,i) + r*math::random_stddev<K>();
	}
	return restrict(dom, y);
}

//---------------------------------------------------------------------------

template<typename K, unsigned int N, template<typename,unsigned int>class Constraint=cartesian_constraint_none>
struct cartesian
:	cartesian_base<K,N>,
	Constraint<K,N>
{};

template<typename K, unsigned int N>
struct tangent_size<cartesian<K,N>> : std::integral_constant<int,static_cast<int>(N)> {};

template<typename K, unsigned int N, template<typename,unsigned int>class Constraint>
struct state_type<cartesian<K,N,Constraint>> {
	typedef typename cartesian_base<K,N>::State type;
};

template<typename K, unsigned int N, template<typename,unsigned int>class Constraint>
struct scalar_type<cartesian<K,N,Constraint>> {
	typedef K type;
};

template<typename K, unsigned int N, template<typename,unsigned int>class Constraint>
void print(
	std::ostream& os,
	const cartesian<K,N,Constraint>&,
	const typename state_type<cartesian<K,N,Constraint>>::type& u
) {
	os << "R^" << N << "[";
	for(size_t i=0; i<N; i++) {
		if(i > 0) {
			os << ", ";
		}
		os << at(u,i);
	}
	os << "]";
}

template<typename K, unsigned int N, template<typename,unsigned int>class Constraint>
typename cartesian_base<K,N>::State exp(
	const cartesian<K,N,Constraint>& dom,
	const typename cartesian_base<K,N>::State& y,
	const typename tangent_type<cartesian<K,N,Constraint>>::type& t
) {
	return restrict(dom, y + t);
}

template<typename K, unsigned int N, template<typename,unsigned int>class Constraint>
typename tangent_type<cartesian<K,N,Constraint>>::type log(
	const cartesian<K,N,Constraint>&,
	const typename cartesian_base<K,N>::State& y,
	const typename cartesian_base<K,N>::State& x
) {
	return x - y;
}

template<typename W, typename K, unsigned int N, template<typename,unsigned int>class Constraint>
typename cartesian_base<K,N>::State mean(
	const cartesian<K,N,Constraint>& dom,
	const std::vector<W>& weights,
	const std::vector<typename cartesian_base<K,N>::State>& states
) {
	BOOST_ASSERT(states.size() == weights.size());
	BOOST_ASSERT(states.size() > 0);
	K weight_sum = static_cast<K>(weights[0]);
	typename cartesian_base<K,N>::State x = static_cast<K>(weights[0])*states[0];
	const std::size_t n = weights.size();
	for(std::size_t i=1; i<n; i++) {
		K w = static_cast<K>(weights[i]);
		BOOST_ASSERT(w >= 0);
		x += w*states[i];
		weight_sum += w;
	}
	BOOST_ASSERT(weight_sum >= 0);
	x *= (K(1) / weight_sum);
	return restrict(dom, x);
}

template<typename W, typename K, unsigned int N, template<typename,unsigned int>class Constraint>
typename cartesian_base<K,N>::State lerp(
	const cartesian<K,N,Constraint>& dom,
	W p,
	const typename cartesian_base<K,N>::State& a,
	const typename cartesian_base<K,N>::State& b
) {
	// FIXME faster!
	return mean(dom,
		std::vector<W>{static_cast<W>(1) - p, p},
		std::vector<typename cartesian_base<K,N>::State>{a, b});
}

}}
//---------------------------------------------------------------------------
#endif
