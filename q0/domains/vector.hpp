#ifndef Q0_DOMAINS_VECTOR_H_
#define Q0_DOMAINS_VECTOR_H_
#include <q0/common.hpp>
#include <q0/math.hpp>
#include <q0/domains.hpp>
#include <vector>
//---------------------------------------------------------------------------
namespace q0 { namespace domains {

/** Using a std::vector as a q0 domain */
using std::vector;

template<typename X>
unsigned int dimension(const vector<X>& dom) {
	// return std::accumulate(dom.begin(), dom.end(), 0,
	//  	[](unsigned int a, const X& x) {
	//  		return a + dimension(x);
	//  	});
	unsigned int a = 0;
	for(const auto& x : dom) {
		a += dimension(x);
	}
	return a;
}

template<typename X>
struct state_type<vector<X>> {
	typedef vector<typename state_type<X>::type> type;
};

template<typename X>
struct scalar_type<vector<X>> {
	typedef typename scalar_type<X>::type type;
};

template<typename X>
void print(std::ostream& os, const vector<X>& dom, const typename state_type<vector<X>>::type& u) {
	BOOST_ASSERT(dom.size() == u.size());
	os << "vector[";
	for(std::size_t i=0; i<u.size(); i++) {
		if(i > 0) {
			os << ", ";
		}
		//os << i << "=";
		print(os, dom[i], u[i]);
	}
	os << "]";
}

namespace detail
{
	template<typename X>
	unsigned int get_tangent_part_position(const vector<X>& dom, std::size_t n) {
		unsigned int p = 0;
		for(std::size_t i=0; i<n; i++) {
			p += dimension(dom[i]);
		}
		return p;
	}

	template<typename X>
	typename tangent_type<X>::type get_tangent_part(const vector<X>& dom, const typename tangent_type<vector<X>>::type& t, std::size_t i) {
		BOOST_ASSERT(i < dom.size());
		const unsigned int p = get_tangent_part_position(dom, i);
		const unsigned int dim_i = dimension(dom[i]);
		return t.segment(p, dim_i);
	}
	
	template<typename X>
	void set_tangent_part(const vector<X>& dom, typename tangent_type<vector<X>>::type& t, std::size_t i, const typename tangent_type<X>::type& x) {
		BOOST_ASSERT(i < dom.size());
		const unsigned int p = get_tangent_part_position(dom, i);
		const unsigned int dim_i = dimension(dom[i]);
		BOOST_ASSERT(dim_i == x.size());
		t.segment(p, dim_i) = x;
	}
}

template<typename X>
typename state_type<vector<X>>::type exp(const vector<X>& dom, const typename state_type<vector<X>>::type& x, const typename tangent_type<vector<X>>::type& t) {
	BOOST_ASSERT(dom.size() == x.size());
	typename state_type<vector<X>>::type u(dom.size());
	for(std::size_t i=0; i<dom.size(); i++) {
		u[i] = domains::exp(dom[i], x[i], detail::get_tangent_part(dom, t, i));
	}
	return u;
}

template<typename X>
typename tangent_type<vector<X>>::type log(const vector<X>& dom, const typename state_type<vector<X>>::type& x, const typename state_type<vector<X>>::type& y) {
	BOOST_ASSERT(dom.size() == x.size());
	BOOST_ASSERT(dom.size() == y.size());
	typename tangent_type<vector<X>>::type t(dimension(dom));
	for(std::size_t i=0; i<dom.size(); i++) {
		detail::set_tangent_part(dom, t, i, domains::log(dom[i], x[i], y[i]));
	}
	return t;
}

template<typename X>
typename state_type<vector<X>>::type random(const vector<X>& dom) {
	typename state_type<vector<X>>::type x(dom.size());
	for(std::size_t i=0; i<dom.size(); i++) {
		x[i] = random(dom[i]);
	}
	return x;
}

template<typename X>
typename state_type<vector<X>>::type random_neighbour(const vector<X>& dom, const typename state_type<vector<X>>::type& x, typename scalar_type<X>::type radius) {
	typename state_type<vector<X>>::type y(dom.size());
	for(std::size_t i=0; i<dom.size(); i++) {
		y[i] = random_neighbour(dom[i], x[i], radius);
	}
	return y;
}

template<typename X, typename W>
typename state_type<vector<X>>::type mean(const vector<X>& dom, const std::vector<W>& weights, const std::vector<typename state_type<vector<X>>::type>& states) {
	typename state_type<vector<X>>::type y(dom.size());
	for(std::size_t i=0; i<dom.size(); i++) {
		std::vector<typename state_type<X>::type> parts(states.size());
		for(std::size_t j=0; j<parts.size(); j++) {
			parts[j] = states[j][i];
		}
		y[i] = mean(dom[i], weights, parts);
	}
	return y;
}

template<typename X, typename W>
typename state_type<vector<X>>::type lerp(const vector<X>& dom, W p, const typename state_type<vector<X>>::type& a, const typename state_type<vector<X>>::type& b) {
	typename state_type<vector<X>>::type y(dom.size());
	for(std::size_t i=0; i<dom.size(); i++) {
		y[i] = lerp(dom[i], p, a[i], b[i]);
	}
	return y;
}

}}
//---------------------------------------------------------------------------
#endif
