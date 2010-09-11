/*
 * Problem05.cpp
 *
 *  Created on: Sep 11, 2010
 *      Author: david
 */

#include "Test.h"
#include "Benchmarks/PointCloudRegistration.h"
#include <QuestZero/Space/Cartesian.h>
#include <QuestZero/Space/SO3.h>
#include <QuestZero/Space/TypelistSpace.h>
#include <QuestZero/IFunction.h>
#include <Danvil/LinAlg.h>
#include <Danvil/Tools/Small.h>
#include <boost/bind.hpp>
#include <iostream>
using std::cout;
using std::endl;

namespace Problem05
{

	typedef Danvil::ctLinAlg::Vec3<double> state0;
	typedef Danvil::ctLinAlg::TQuaternion<double> state1;
	typedef LOKI_TYPELIST_2(state0,state1) state_types;
	typedef Spaces::TypelistState<state_types> state;

	typedef Spaces::Cartesian::CartesianSpace<state0> space0;
	typedef Spaces::SO3::SO3Space<double> space1;
	typedef LOKI_TYPELIST_2(space0,space1) space_types;
	typedef Spaces::TypelistSpace<space_types, state> space;

	typedef FunctionFromDelegate<state> function;

	space FactorSpace() {
		space myspace;
		myspace.space<0>().setDomainRange(state0(3,4,5));
		return myspace;
	}

	class RegistrationFunction
	: public IFunction<state>,
	  public Benchmarks::PointCloudRegistration<double>
	{
	public:
		RegistrationFunction(size_t n)
		: Benchmarks::PointCloudRegistration<double>(n) {}

		double operator()(const state& state) {
			return fit(state.part<1>().rotation(), state.part<0>());
		}
	};

	typedef RegistrationFunction Function;

	PTR(Function) FactorFunction() {
		return Danvil::Ptr(new RegistrationFunction(100));
	}

	void run()
	{
		cout << "----- Registration w. position (TypelistSpace) -----" << endl;
		TestProblem(FactorSpace(), FactorFunction());
	}
}
