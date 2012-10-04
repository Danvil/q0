#ifndef Q0_ALGORITHMS_HPP_
#define Q0_ALGORITHMS_HPP_
#include <q0/domains.hpp>
#include <q0/objective.hpp>
#include <q0/common.hpp>
#include <functional>
//---------------------------------------------------------------------------
namespace q0 {

template<template<typename,typename,typename,typename> class Algorithm>
struct minimize
{
	template<typename Domain, typename Objective, typename Control, typename Compare=std::less<typename objective::result_type<Objective>::type>>
	static inline particle<typename domains::state_type<Domain>::type,typename objective::result_type<Objective>::type>
	apply(const Domain& domain, Objective f, Control control, Compare cmp=Compare()) {
		// FIXME check typename domains::state_type<Domain>::type check1;
		// FIXME check typename objective::result_type<Objective>::type check2;
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
