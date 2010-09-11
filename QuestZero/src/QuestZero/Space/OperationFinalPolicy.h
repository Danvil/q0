/*
 * OperationFinalPolicies.h
 *
 *  Created on: Sep 10, 2010
 *      Author: david
 */

#ifndef OPERATIONFINALPOLICIES_H_
#define OPERATIONFINALPOLICIES_H_
//---------------------------------------------------------------------------
namespace Spaces {
//---------------------------------------------------------------------------

namespace OperationFinalPolicy
{
	template<typename State>
	struct Unprojected
	{
		template<class Space>
		const State& opFinal(const Space&, const State& state) const { return state; }

	protected:
		~Unprojected() {}
	};

	template<typename State>
	struct Projected
	{
		template<class Space>
		State opFinal(const Space& space, const State& state) const { return space.project(state); }

	protected:
		~Projected() {}
	};
}

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
