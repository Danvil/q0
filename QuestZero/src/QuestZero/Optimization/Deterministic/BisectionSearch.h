/*
 * BisectionSearch.h
 *
 *  Created on: Sep 15, 2010
 *      Author: david
 */

#ifndef DETERMINISTIC_BISECTIONSEARCH_H_
#define DETERMINISTIC_BISECTIONSEARCH_H_

#include <cmath>

/** Finds the root of a *monoton* function defined on a subset [min,max] of the real numbers */
struct BisectionRootFinder
{
	template<class FunctionT>
	static typename FunctionT::StateT Solve(const FunctionT& f, typename FunctionT::StateT min, typename FunctionT::StateT max, typename FunctionT::StateT epsilon)
	{
		typedef typename FunctionT::ResultT ResultT;
		typedef typename FunctionT::StateT StateT;
		StateT left = min;
		StateT midpoint = left;
		StateT right = max;

		ResultT f_left = f(left);
		ResultT f_right = f(right);

		unsigned long cnt = 0;

		while(std::abs(right - left) > 2 * epsilon) {
			cnt ++;
			// Calculate midpoint of domain
			midpoint = (right + left) / 2;
			ResultT f_midpoint = f(midpoint); //f.F(midpoint);
			// Find f(midpoint)
			if(f_left * f_midpoint < 0) {
				// Throw away right half
				right = midpoint;
				f_right = f_midpoint;
			} else if(f_right * f_midpoint < 0) {
				// Throw away left half
				left = midpoint;
				f_left = f_midpoint;
			} else {
				// Our midpoint is exactly on the root
				return midpoint;
			}
		}

		return (right + left) / 2; // should be more accurate than right or left ...
	}
};

#endif
