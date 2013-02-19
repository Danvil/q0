#ifndef Q0_ALGORITHMS_HPP_
#define Q0_ALGORITHMS_HPP_
#include "domains.hpp"
#include "objective.hpp"
#include "common.hpp"
#include <type_traits>
//---------------------------------------------------------------------------
namespace q0 {

// template<template<typename,typename,typename,typename> class Algorithm>
// struct minimize
// {
// 	template<
// 		typename Domain,
// 		typename Objective,
// 		typename Control,
// 		typename Compare=std::less<typename objective::result_of<Domain,Objective>::type>
// 	>
// 	static inline particle<
// 		typename domains::state_type<Domain>::type,
// 		typename objective::result_of<Domain,Objective>::type
// 	>
// 	apply(const Domain& domain, Objective f, Control control, Compare cmp=Compare()) {
// 		// FIXME check typename domains::state_type<Domain>::type check1;
// 		// FIXME check typename objective::result_type<Objective>::type check2;
// 		return Algorithm<Domain,Objective,Control,Compare>::apply(domain, f, control, cmp);
// 	}
// };

template<
	typename Domain,
	typename Objective,
	typename Algorithm, 
	typename Control
>
typename problem_traits<Domain,Objective>::particle_t
minimize(const Domain& dom, Objective f, Algorithm alg, Control ctrl)
{
	typedef typename problem_traits<Domain,Objective>::score_t score_t;
	return alg.solve(dom, f, ctrl, std::less<score_t>());
} 

}
//---------------------------------------------------------------------------
#endif
