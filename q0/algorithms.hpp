#ifndef Q0_ALGORITHMS_HPP_
#define Q0_ALGORITHMS_HPP_
#include <q0/algorithms/random_search.hpp>
//---------------------------------------------------------------------------
namespace q0 {

template<template<typename,typename,typename,typename> class Algorithm>
struct minimize
{
	template<typename Domain, typename Objective, typename Control, typename Compare=std::less<typename objective::result_type<Objective>::type>>
	static inline particle<typename domains::state_type<Domain>::type,typename objective::result_type<Objective>::type>
	apply(const Domain& domain, Objective f, Control control, Compare cmp=Compare()) {
		return Algorithm<Domain,Objective,Control,Compare>::apply(domain, f, control, cmp);
	}
};

// template<template<class,class,class,class> Algorithm, typename Domain, typename Objective, typename Control, typename Compare=std::less<typename objective::result_type<Objective>::type>>
// particle<typename domains::state_type<Domain>::type,typename objective::result_type<Objective>::type>
// minimize(const Domain& domain, Objective f, Control control, Compare cmp=Compare()) {
// 	return Algorithm<Domain,Objective,Control,Compare>::apply(domain, f, control, cmp);
// }

}
//---------------------------------------------------------------------------
#endif
