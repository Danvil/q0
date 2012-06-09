/*
 * SpecialFunctions_3.cpp
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

typedef Eigen::Matrix<float, 2, 1> state_t;

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

	std::cout << "Optimizing in space [-10.0|+10.0]^3" << std::endl;
	state_t range(10, 10);
	Q0::Spaces::FiniteCartesianSpace<state_t> space;
	space.setDomainRange(range);

	std::cout << std::endl;
	std::cout << "----- Null --- Expected result: (0,0) -----" << std::endl;
	std::cout << std::endl;
	Solve(space, &Q0::SpecialFunctions<state_t>::Null, p_num_particles, p_print_result_state);

	std::cout << std::endl;
	std::cout << "----- Sphere --- Expected result: (0,0) -----" << std::endl;
	std::cout << std::endl;
	Solve(space, &Q0::SpecialFunctions<state_t>::Sphere, p_num_particles, p_print_result_state);

	std::cout << std::endl;
	std::cout << "----- DiscreetSphere --- Expected result: (0,0) -----" << std::endl;
	std::cout << std::endl;
	Solve(space, &Q0::SpecialFunctions<state_t>::DiscreetSphere, p_num_particles, p_print_result_state);

	std::cout << std::endl;
	std::cout << "----- Schwefel2_21 --- Expected result: (-inf,-inf) -----" << std::endl;
	std::cout << std::endl;
	Solve(space, &Q0::SpecialFunctions<state_t>::Schwefel2_21, p_num_particles, p_print_result_state);

	std::cout << std::endl;
	std::cout << "----- Schwefel2_22 --- Expected result: (0,0) -----" << std::endl;
	std::cout << std::endl;
	Solve(space, &Q0::SpecialFunctions<state_t>::Schwefel2_22, p_num_particles, p_print_result_state);

	std::cout << std::endl;
	std::cout << "----- Schwefel1_2 --- Expected result: (inf,inf) -----" << std::endl;
	std::cout << std::endl;
	Solve(space, &Q0::SpecialFunctions<state_t>::Schwefel1_2, p_num_particles, p_print_result_state);

	std::cout << std::endl;
	std::cout << "----- Rosenbrock --- Expected result: (1,1) -----" << std::endl;
	std::cout << std::endl;
	Solve(space, &Q0::SpecialFunctions<state_t>::Rosenbrock, p_num_particles, p_print_result_state);

	std::cout << std::endl;
	std::cout << "----- Himmelblau --- Expected result: (3,2), (-2.805118,3.131312), (-3.779310,-3.283186), (3.584428,-1.848126) -----" << std::endl;
	std::cout << std::endl;
	Solve(space, &Q0::SpecialFunctions<state_t>::Himmelblau, p_num_particles, p_print_result_state);

	std::cout << std::endl;
	std::cout << "----- Rastrigin --- Expected result: (0,0) -----" << std::endl;
	std::cout << std::endl;
	Solve(space, &Q0::SpecialFunctions<state_t>::Rastrigin, p_num_particles, p_print_result_state);

	std::cout << std::endl;
	std::cout << "----- Step --- Expected result: (0,0) -----" << std::endl;
	std::cout << std::endl;
	Solve(space, &Q0::SpecialFunctions<state_t>::Step, p_num_particles, p_print_result_state);

	std::cout << std::endl;
	std::cout << "----- Ackley --- Expected result: (0,0) -----" << std::endl;
	std::cout << std::endl;
	Solve(space, &Q0::SpecialFunctions<state_t>::Ackley, p_num_particles, p_print_result_state);

	std::cout << std::endl;
	std::cout << "----- Griewank --- Expected result: (0,0) -----" << std::endl;
	std::cout << std::endl;
	Solve(space, &Q0::SpecialFunctions<state_t>::Griewank, p_num_particles, p_print_result_state);

	return 1;
}
