/*
 * SpecialFunctions_31.cpp
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
#include <iostream>

typedef Eigen::Matrix<float, 31, 1> state_t;

template<typename State>
double shiftedMinima(boost::function<double(const State&)> f, const State& shift, const State& x) {
	return f(x - shift);
}

int main(int argc, char* argv[])
{
	std::cout << "Optimizing in space [-10.0|+10.0]^31" << std::endl;
	state_t range;
	for(unsigned int i=0; i<range.rows(); i++) {
		range[i] = 10;
	}
	Q0::Spaces::Cartesian::FiniteCartesianSpace<state_t> space;
	space.setDomainRange(range);

	Q0::Functions::BoostFunctionSingleWrapper<state_t,double> f;
	state_t minima;
	for(unsigned int i=0; i<minima.rows(); i++) {
		minima[i] = 0.1 * (double)i;
	}
	std::cout << "Expected result for all functions is: " << minima.transpose() << std::endl;

	std::cout << "----- Benchmark: DiscreetSphere -----" << std::endl;
	f.set_functor([&minima](const state_t& x) {
		return Benchmarks::Cartesian<state_t>::DiscreetSphere(minima - x);
	});
	TestProblem(space, f);

	std::cout << "----- Benchmark: Schwefel2_21 -----" << std::endl;
	f.set_functor([&minima](const state_t& x) {
		return Benchmarks::Cartesian<state_t>::Schwefel2_21(minima - x);
	});
	TestProblem(space, f);

	std::cout << "----- Benchmark: Rosenbrock -----" << std::endl;
	f.set_functor([&minima](const state_t& x) {
		return Benchmarks::Cartesian<state_t>::Rosenbrock(minima - x);
	});
	TestProblem(space, f);

	std::cout << "----- Benchmark: Rastrigin -----" << std::endl;
	f.set_functor([&minima](const state_t& x) {
		return Benchmarks::Cartesian<state_t>::Schwefel2_21(minima - x);
	});
	TestProblem(space, f);

	return 1;
}
