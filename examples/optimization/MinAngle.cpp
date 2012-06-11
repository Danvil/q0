/*
 * MinAngle.cpp
 *
 *  Created on: May 31, 2012
 *      Author: david
 */

#include "Solve.h"
#include <QuestZero/Optimization/Optimization.h>
#include <QuestZero/Spaces/Angular.h>
#include <boost/bind.hpp>
#include <iostream>
using namespace Q0;

typedef double state_t;
typedef double score_t;

score_t function(state_t a) {
	state_t q = 0.63 * boost::math::constants::pi<state_t>() - a;
	return q * q;
}

int main(int argc, char* argv[])
{

	unsigned int p_num_particles = 10000;
	bool p_verbose = true;

	std::cout << "----- Find optimal angle (full interval) -----" << std::endl;
	std::cout << "Optimum is: " << 0.63 * boost::math::constants::pi<state_t>() << std::endl;
	std::cout << std::endl;

	{
		std::cout << "----- full interval -----" << std::endl;

		Spaces::FullAngularSpace<state_t> space;

		Solve(space, &function, p_num_particles, p_verbose);
	}

	{
		std::cout << "----- interval = [1/4 pi | 3/4 pi] -----" << std::endl;

		Spaces::IntervalAngularSpace<state_t> space;
		space.set_lower(0.25 * boost::math::constants::pi<state_t>());
		space.set_upper(0.75 * boost::math::constants::pi<state_t>());

		Solve(space, &function, p_num_particles, p_verbose);
	}

	{
		std::cout << "----- interval = [1/4 pi | 1/2 pi] -----" << std::endl;

		Spaces::IntervalAngularSpace<state_t> space;
		space.set_lower(0.25 * boost::math::constants::pi<state_t>());
		space.set_upper(0.50 * boost::math::constants::pi<state_t>());

		Solve(space, &function, p_num_particles, p_verbose);
	}

	return 1;
}
