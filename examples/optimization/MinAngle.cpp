/*
 * MinAngle.cpp
 *
 *  Created on: May 31, 2012
 *      Author: david
 */

#include "../Test.h"
#include <QuestZero/Optimization/Optimization.h>
#include <QuestZero/Optimization/Functions.h>
#include <QuestZero/Spaces/Angular.h>
#include <boost/bind.hpp>
#include <iostream>
using namespace Q0;

typedef double real;

double function(real a) {
	double q = 0.63 * boost::math::constants::pi<real>() - a;
	return q * q;
}

int main(int argc, char* argv[])
{

	std::cout << "----- Find optimal angle (full interval) -----" << std::endl;
	std::cout << "Optimum is: " << 0.63 * boost::math::constants::pi<real>() << std::endl;
	std::cout << endl;

	{
		std::cout << "----- full interval -----" << std::endl;

		Spaces::Angular::FullAngularSpace<real> space;

		Functions::BoostFunctionSingleWrapper<real,real> f;
		f.set_functor(boost::bind(&function, _1));

		TestProblem(space, f);
	}

	{
		std::cout << "----- interval = [1/4 pi | 3/4 pi] -----" << std::endl;

		Spaces::Angular::IntervalAngularSpace<real> space;
		space.set_lower(0.25 * boost::math::constants::pi<real>());
		space.set_upper(0.75 * boost::math::constants::pi<real>());

		Functions::BoostFunctionSingleWrapper<real,real> f;
		f.set_functor(boost::bind(&function, _1));

		TestProblem(space, f);
	}

	{
		std::cout << "----- interval = [1/4 pi | 1/2 pi] -----" << std::endl;

		Spaces::Angular::IntervalAngularSpace<real> space;
		space.set_lower(0.25 * boost::math::constants::pi<real>());
		space.set_upper(0.50 * boost::math::constants::pi<real>());

		Functions::BoostFunctionSingleWrapper<real,real> f;
		f.set_functor(boost::bind(&function, _1));

		TestProblem(space, f);
	}

	return 1;
}
