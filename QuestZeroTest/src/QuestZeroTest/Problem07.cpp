/*
 * Problem07.cpp
 *
 *  Created on: Sep 11, 2010
 *      Author: david
 */

#include "Test.h"
#include <QuestZero/Spaces/Cartesian.h>
#include <QuestZero/Spaces/SO3.h>
#include <QuestZero/Spaces/TypelistSpace.h>
#include <QuestZero/Optimization/Functions.h>
#include <Danvil/LinAlg.h>
#include <Danvil/Tools/MoreMath.h>
#include <boost/bind.hpp>
#include <iostream>
using std::cout;
using std::endl;
using namespace Q0;

namespace Problem07
{

	typedef Danvil::ctLinAlg::Vec3f state0;
	typedef Danvil::SO3::Quaternion<double> state1;
	typedef float state2;
	typedef LOKI_TYPELIST_3(state0,state1,state2) state_types;
	typedef Spaces::TypelistState<state_types> state;

	typedef Spaces::Cartesian::FiniteCartesianSpace<state0> space0;
	typedef Spaces::SO3::SO3Space<double> space1;
	typedef Spaces::Cartesian::IntervalSpace<state2> space2;
	typedef LOKI_TYPELIST_3(space0,space1,space2) space_types;
	typedef Spaces::TypelistSpace<space_types, state> space;

	space FactorSpace() {
		space myspace;
		myspace.space<0>().setDomainRange(state0(3,4,5));
		myspace.space<2>().setDomainRange(10.0f);
		return myspace;
	}

	double sample02(const state& s)
	{
		state0 s1 = s.part<0>();
		state1 s2 = s.part<1>();
		state2 s3 = s.part<2>();
		return std::sqrt(s1.x*s1.x + s1.y*s1.y + s1.z*s1.z) + std::abs(s2.w) + std::abs(s2.x) + std::abs(s2.y) + std::abs(s2.z) + std::abs(s3);
	}

	typedef Functions::BoostFunctionSingleWrapper<state> function;

	function FactorFunction() {
		function f;
		f.set_functor(boost::bind(&sample02, _1));
		return f;
	}

	void run()
	{
		cout << "----- TypelistSpace -----" << endl;
		TestProblem(FactorSpace(), FactorFunction());
	}
}
