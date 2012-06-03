/*
 * SpecialFunctions_31.cpp
 *
 *  Created on: May 30, 2012
 *      Author: david
 */

#include "Solve.h"
#include "SpecialFunctions.h"
#include <QuestZero/Spaces/Cartesian.h>
#include <Eigen/Dense>
#include <boost/program_options.hpp>
#include <iostream>

typedef Eigen::Matrix<float, 31, 1> state_t;

int main(int argc, char* argv[])
{
	unsigned int p_num_particles = 16384;
	bool p_print_result_state = false;
	{
		namespace po = boost::program_options;
		po::options_description desc("Allowed options");
		desc.add_options()
			("help", "produce help message")
			("num_particles", po::value<unsigned int>(&p_num_particles), "number of particles to use")
			("verbose", po::value<bool>(&p_print_result_state), "print result vectors")
		;

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);
	}

	std::cout << "Optimizing in space [-10.0|+10.0]^31" << std::endl;
	state_t range;
	for(unsigned int i=0; i<range.rows(); i++) {
		range[i] = 10;
	}
	Q0::Spaces::Cartesian::FiniteCartesianSpace<state_t> space;
	space.setDomainRange(range);

	std::cout << std::endl;
	std::cout << "----- Sphere --- Expected result: 0 -----" << std::endl;
	std::cout << std::endl;
	Solve(space, &Q0::SpecialFunctions<state_t>::Sphere, p_num_particles, p_print_result_state);

	std::cout << std::endl;
	std::cout << "----- DiscreetSphere --- Expected result: 0 -----" << std::endl;
	std::cout << std::endl;
	Solve(space, &Q0::SpecialFunctions<state_t>::DiscreetSphere, p_num_particles, p_print_result_state);

	std::cout << std::endl;
	std::cout << "----- Schwefel2_21 --- Expected result: -inf -----" << std::endl;
	std::cout << std::endl;
	Solve(space, &Q0::SpecialFunctions<state_t>::Schwefel2_21, p_num_particles, p_print_result_state);

	return 1;
}
