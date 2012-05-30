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
#include <Danvil/LinAlg.h>
#include <boost/bind.hpp>
#include <iostream>
using std::cout;
using std::endl;
using namespace Q0;

typedef Danvil::ctLinAlg::Vec<double, 31> state_t;

typedef Spaces::Cartesian::FiniteCartesianSpace<state_t> space_t;

space_t FactorSpace() {
	space_t space;
	state_t range;
	for(unsigned int i=0; i<state_t::Dimension; i++) {
		range[i] = 10;
	}
	space.setDomainRange(range);
	return space;
}

typedef Functions::BoostFunctionSingleWrapper<state_t,double> Function;

template<typename State>
double shiftedMinima(boost::function<double(const State&)> f, const State& shift, const State& x) {
	return f(x - shift);
}

Function FactorFunction(boost::function<double(const state_t&)> f1) {
	Function f;
	state_t minima;
	for(unsigned int i=0; i<state_t::Dimension; i++) {
		minima[i] = 0.1 * (double)i;
	}
	f.set_functor(boost::bind(&shiftedMinima<state_t>, f1, minima, _1));
	return f;
}

int main(int argc, char* argv[])
{
	cout << "----- Cartesian Vec<double,31> --- DiscreetSphere -----" << endl;
	TestProblem(FactorSpace(), FactorFunction(boost::bind(&Benchmarks::Cartesian<state_t>::DiscreetSphere, _1)));
	cout << "----- Cartesian Vec<double,31> --- Schwefel2_21 -----" << endl;
	TestProblem(FactorSpace(), FactorFunction(boost::bind(&Benchmarks::Cartesian<state_t>::Schwefel2_21, _1)));
	cout << "----- Cartesian Vec<double,31> --- Rosenbrock -----" << endl;
	TestProblem(FactorSpace(), FactorFunction(boost::bind(&Benchmarks::Cartesian<state_t>::Rosenbrock, _1)));
	cout << "----- Cartesian Vec<double,31> --- Rastrigin -----" << endl;
	TestProblem(FactorSpace(), FactorFunction(boost::bind(&Benchmarks::Cartesian<state_t>::Rastrigin, _1)));

	return 1;
}
