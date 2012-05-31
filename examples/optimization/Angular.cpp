/*
 * Angular.cpp
 *
 *  Created on: May 31, 2012
 *      Author: david
 */

#include "../Test.h"
#include <QuestZero/Optimization/Optimization.h>
#include <QuestZero/Optimization/Functions.h>
#include <QuestZero/Spaces/Angular.h>
#include <Danvil/LinAlg.h>
#include <Danvil/Print.h>
#include <boost/bind.hpp>
#include <iostream>
using std::cout;
using std::endl;
using namespace Q0;

typedef double real;

double function(real a) {
	double q = 0.63 * boost::math::constants::pi<real>() - a;
	return q * q;
}

int main(int argc, char* argv[])
{
	{
		cout << "----- Find optimal angle (full interval) -----" << endl;

		Spaces::Angular::FullAngularSpace<real> space;

		Functions::BoostFunctionSingleWrapper<real,real> f;
		f.set_functor(boost::bind(&function, _1));

		TestProblem(space, f);
	}

	{
		cout << "----- Find optimal angle (interval = [1/4 pi | 3/4 pi]) -----" << endl;

		Spaces::Angular::IntervalAngularSpace<real> space;
		space.set_lower(0.25 * boost::math::constants::pi<real>());
		space.set_upper(0.75 * boost::math::constants::pi<real>());

		Functions::BoostFunctionSingleWrapper<real,real> f;
		f.set_functor(boost::bind(&function, _1));

		TestProblem(space, f);
	}

	{
		cout << "----- Find optimal angle (interval = [1/4 pi | 1/2 pi]) -----" << endl;

		Spaces::Angular::IntervalAngularSpace<real> space;
		space.set_lower(0.25 * boost::math::constants::pi<real>());
		space.set_upper(0.50 * boost::math::constants::pi<real>());

		Functions::BoostFunctionSingleWrapper<real,real> f;
		f.set_functor(boost::bind(&function, _1));

		TestProblem(space, f);
	}

	return 1;
}
