/*
 * OperationFinalPolicies.h
 *
 *  Created on: Sep 10, 2010
 *      Author: david
 */

#ifndef OPERATIONFINALPOLICIES_H_
#define OPERATIONFINALPOLICIES_H_

namespace Spaces {

namespace OperationFinalPolicy
{
	template<typename State>
	struct Unprojected
	{
		const State& opFinal(const State& state) const { return state; }
	};

	template<typename State>
	struct Projected
	{
		State opFinal(const State& state) const { return project(state); }
	};
}

}

#endif
