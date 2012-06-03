/*
 * Problem07.cpp
 *
 *  Created on: Sep 11, 2010
 *      Author: david
 */

#include "Solve.h"
#include <QuestZero/Spaces/Cartesian.h>
#include <QuestZero/Spaces/SO3.h>
#include <QuestZero/Spaces/TypelistSpace.h>
#include <QuestZero/Optimization/Functions.h>
#include <boost/bind.hpp>
#include <iostream>

typedef Eigen::Vector3f state0;
typedef Eigen::Quaternion<double> state1;
typedef float state2;
typedef Q0::Spaces::TypelistState<LOKI_TYPELIST_3(state0,state1,state2)> state;

typedef Q0::Spaces::Cartesian::FiniteCartesianSpace<state0> space0;
typedef Q0::Spaces::SO3::FullSO3Space<double> space1;
typedef Q0::Spaces::Cartesian::IntervalSpace<state2> space2;
typedef Q0::Spaces::TypelistSpace<LOKI_TYPELIST_3(space0,space1,space2), state> space;

double sample02(const state& s)
{
	state0 s1 = s.part<0>();
	state1 s2 = s.part<1>();
	state2 s3 = s.part<2>();
	return s1.norm() + std::abs(s2.w()) + std::abs(s2.x()) + std::abs(s2.y()) + std::abs(s2.z()) + std::abs(s3);
}

int main(int argc, char** argv)
{
	std::cout << "----- TypelistSpace -----" << std::endl;

	space myspace;
	myspace.space<0>().setDomainRange(state0(3,4,5));
	myspace.space<2>().setDomainRange(10.0f);

	Q0::Functions::BoostFunctionSingleWrapper<state> f;
	f.set_functor(boost::bind(&sample02, _1));

	TestProblem(myspace, f);

	return 1;
}
