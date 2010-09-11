/*
 * Problem05.cpp
 *
 *  Created on: Sep 11, 2010
 *      Author: david
 */

#include "Test.h"
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

	typedef Danvil::ctLinAlg::Vec3f state0;
	typedef Danvil::ctLinAlg::TQuaternion<double> state1;
	typedef float state2;
	typedef LOKI_TYPELIST_3(state0,state1,state2) state_types;
	typedef Spaces::TypelistState<state_types> state;

	typedef Spaces::Cartesian::CartesianSpace<state0> space0;
	typedef Spaces::SO3::SO3Space<double> space1;
	typedef Spaces::Cartesian::CartesianSpace<state2, Spaces::Cartesian::Operations::Linear<state2,float>, Spaces::Cartesian::Domains::Interval<state2> > space2;
	typedef LOKI_TYPELIST_3(space0,space1,space2) space_types;
	typedef Spaces::TypelistSpace<space_types, state> space;

	typedef FunctionFromDelegate<state> function;

	space FactorSpace() {
		space myspace;
		myspace.space<0>().setDomainRange(state0(3,4,5));
		myspace.space<2>().setDomainRange(10);
		return myspace;
	}

	double sample02(const state& s)
	{
		state0 s1 = s.part<0>();
		state1 s2 = s.part<1>();
		state2 s3 = s.part<2>();
		return sqrt(s1.x*s1.x + s1.y*s1.y + s1.z*s1.z) + Danvil::abs(s2.w) + Danvil::abs(s2.x) + Danvil::abs(s2.y) + Danvil::abs(s2.z) + Danvil::abs(s3);
	}

	PTR(function) FactorFunction() {
		return Danvil::Ptr(new function(boost::bind(&sample02, _1)));
	}

	void run()
	{
		cout << "----- TypelistSpace -----" << endl;
		TestProblem(FactorSpace(), FactorFunction());
	}
}
