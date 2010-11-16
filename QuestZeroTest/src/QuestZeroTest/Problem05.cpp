/*
 * Problem05.cpp
 *
 *  Created on: Sep 11, 2010
 *      Author: david
 */

#include "Test.h"
#include "Benchmarks/PointCloudRegistration.h"
#include <QuestZero/Spaces/Cartesian.h>
#include <QuestZero/Spaces/SO3.h>
#include <QuestZero/Spaces/TypelistSpace.h>
#include <QuestZero/Optimization/Functions.h>
#include <Danvil/LinAlg.h>
#include <Danvil/SO3.h>
#include <boost/bind.hpp>
#include <iostream>
using std::cout;
using std::endl;

namespace Problem05
{

	typedef Danvil::ctLinAlg::Vec3<double> state0;
	typedef Danvil::SO3::Quaternion<double> state1;
	typedef LOKI_TYPELIST_2(state0,state1) state_types;
	typedef Spaces::TypelistState<state_types> state;

	typedef Spaces::Cartesian::CartesianSpace<state0> space0;
	typedef Spaces::SO3::SO3Space<double> space1;
	typedef LOKI_TYPELIST_2(space0,space1) space_types;
	typedef Spaces::TypelistSpace<space_types, state> space;

	space FactorSpace() {
		space myspace;
		myspace.space<0>().setDomainRange(state0(3,4,5));
		return myspace;
	}

	struct RegistrationFunction
	: public Benchmarks::PointCloudRegistration<double>
	{
		double operator()(const state& state) const {
			return fit(Danvil::SO3::ConvertToMatrix(state.part<1>()), state.part<0>());
		}
	};

	typedef Functions::AddParallel<state, RegistrationFunction> function;

	function FactorFunction() {
		function f;
		f.createRandomProblem(100);
		return f;
	}

	void run()
	{
		cout << "----- Registration w. position (TypelistSpace) -----" << endl;
		TestProblem(FactorSpace(), FactorFunction());
	}
}
