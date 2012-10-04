#ifndef Q0_DOMAINS_HPP_
#define Q0_DOMAINS_HPP_
#include <vector>
//---------------------------------------------------------------------------
namespace q0 { namespace domains {

template<typename Domain>
struct state_type;

template<typename Domain>
struct tangent_type;

template<typename Domain>
struct scalar_type;

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
template<typename Domain>
typename state_type<Domain>::type lerp(const Domain& dom, typename scalar_type<Domain>::type p, const typename state_type<Domain>::type& a, const typename state_type<Domain>::type& b) {
	return mean(dom, {static_cast<typename scalar_type<Domain>::type>(1) - p, p}, {a, b});
}

}}
//---------------------------------------------------------------------------
#endif
