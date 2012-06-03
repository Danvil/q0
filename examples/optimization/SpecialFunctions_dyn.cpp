/*
 * SpecialFunctions_dyn.cpp
 *
 *  Created on: May 30, 2012
 *      Author: david
 */

#include "../Test.h"
#include "../Benchmarks/Cartesian.h"
#include <QuestZero/Optimization/Optimization.h>
#include <QuestZero/Optimization/Functions.h>
#include <QuestZero/Spaces/Cartesian.h>
#include <Eigen/Dense>
#include <boost/bind.hpp>
#include <boost/program_options.hpp>
#include <iostream>

typedef Eigen::Matrix<float, Eigen::Dynamic, 1> state_t;

template<typename State>
double shiftedMinima(boost::function<double(const State&)> f, const State& shift, const State& x) {
	return f(x - shift);
}

int main(int argc, char* argv[])
{
	unsigned int p_dim = 10;
	unsigned int p_num_particles = 16384;
	bool p_print_result_state = false;
	{
		namespace po = boost::program_options;
		po::options_description desc("Allowed options");
		desc.add_options()
			("help", "produce help message")
			("dim", po::value<unsigned int>(&p_dim), "dimensionality of state space")
			("num_particles", po::value<unsigned int>(&p_num_particles), "number of particles to use")
			("verbose", po::value<bool>(&p_print_result_state), "print result vectors")
		;

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);
	}

	std::cout << "Optimizing in space [-10.0|+10.0]^" << p_dim << std::endl;
	state_t range(p_dim);
	for(unsigned int i=0; i<range.rows(); i++) {
		range[i] = 10;
	}
	Q0::Spaces::Cartesian::FiniteCartesianSpace<state_t> space;
	space.setDomainRange(range);

	Q0::Functions::BoostFunctionSingleWrapper<state_t,double> f;

	std::cout << std::endl;
	std::cout << "----- Sphere --- Expected result: 0 -----" << std::endl;
	std::cout << std::endl;
	f.set_functor([](const state_t& x) {
		return Benchmarks::Cartesian<state_t>::Sphere(x);
	});
	TestProblem(space, f, p_num_particles, p_print_result_state);

	std::cout << std::endl;
	std::cout << "----- DiscreetSphere --- Expected result: 0 -----" << std::endl;
	std::cout << std::endl;
	f.set_functor([](const state_t& x) {
		return Benchmarks::Cartesian<state_t>::DiscreetSphere(x);
	});
	TestProblem(space, f, p_num_particles, p_print_result_state);

	std::cout << std::endl;
	std::cout << "----- Schwefel2_21 --- Expected result: -inf -----" << std::endl;
	std::cout << std::endl;
	f.set_functor([](const state_t& x) {
		return Benchmarks::Cartesian<state_t>::Schwefel2_21(x);
	});
	TestProblem(space, f, p_num_particles, p_print_result_state);

	return 1;
}
