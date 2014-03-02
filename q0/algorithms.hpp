#ifndef Q0_ALGORITHMS_HPP_
#define Q0_ALGORITHMS_HPP_
#include "domains.hpp"
#include "objective.hpp"
#include "common.hpp"
#include <type_traits>
//---------------------------------------------------------------------------
namespace q0 {

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
