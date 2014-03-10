#ifndef Q0_DOMAINS_HPP_
#define Q0_DOMAINS_HPP_
#include <q0/common.hpp>
#include <vector>
#include <type_traits>
//---------------------------------------------------------------------------
namespace q0 { namespace domains {

/** Per default dimension specifies a domain with dynamic dimension */
template<typename Domain>
struct tangent_size : std::integral_constant<int,-1> {}; // WARNING this constants needs to be the same as Eigen::Dynamic!

/** If the domain has a dynamic dimension, this function must be overloaded and return the correct dimension */
template<typename Domain>
typename std::enable_if<(tangent_size<Domain>::value>0), unsigned int>::type
dimension(const Domain&) {
	return static_cast<unsigned int>(tangent_size<Domain>::value); // value is positive!
}

/** The type of domain states */
template<typename Domain>
struct state_type;

/** The scalar type of the domain */
template<typename Domain>
struct state_scalar_type;

/** The type of tangent space vectors */
template<typename Domain>
struct tangent_type {
	typedef typename tangent_vec<typename state_scalar_type<Domain>::type,tangent_size<Domain>::value>::type type;
};

/** Prototype for function 'exp' */
//template<typename Domain>
//typename state_type<Domain>::type exp(const Domain& dom, const typename state_type<Domain>::type& x, const typename tangent_type<Domain>::type& t);

/** Prototype for function 'log' */
//template<typename Domain>
//typename tangent_type<Domain>::type log(const Domain& dom, const typename state_type<Domain>::type& x, const typename state_type<Domain>::type& y);

/** Gets a vector of random states */
template<typename Domain>
std::vector<typename state_type<Domain>::type> random(const Domain& dom, unsigned int n) {
	std::vector<typename state_type<Domain>::type> states(n);
	for(std::size_t i=0; i<states.size(); i++) {
		states[i] = random(dom);
	}
	return states;
}

/** Computes the weighted mean between two points
 * Identical to mean(dom,{1-p,p},{a,b})
 */
//template<typename W, typename Domain>
//typename state_type<Domain>::type lerp(const Domain& dom, W p, const typename state_type<Domain>::type& a, const typename state_type<Domain>::type& b) {
//	return mean(dom, std::vector<W>{static_cast<W>(1) - p, p}, {a, b});
//}

}}
//---------------------------------------------------------------------------
#endif
