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

typedef Danvil::ctLinAlg::Vec<double, 31> State;

typedef Spaces::Cartesian::FiniteCartesianSpace<State> Space;

Space FactorSpace() {
	Space space;
	State range;
	for(unsigned int i=0; i<State::Dimension; i++) {
		range[i] = 10;
	}
	space.setDomainRange(range);
	return space;
}

typedef Functions::BoostFunctionSingleWrapper<State,double> Function;

template<typename State>
double shiftedMinima(boost::function<double(const State&)> f, const State& shift, const State& x) {
	return f(x - shift);
}

Function FactorFunction(boost::function<double(const State&)> f1) {
	Function f;
	State minima;
	for(unsigned int i=0; i<State::Dimension; i++) {
		minima[i] = 0.1 * (double)i;
	}
	f.set_functor(boost::bind(&shiftedMinima<State>, f1, minima, _1));
	return f;
}

int main(int argc, char* argv[])
{
	cout << "----- Cartesian Vec<double,31> --- DiscreetSphere -----" << endl;
	TestProblem(FactorSpace(), FactorFunction(boost::bind(&Benchmarks::Cartesian<State>::DiscreetSphere, _1)));
	cout << "----- Cartesian Vec<double,31> --- Schwefel2_21 -----" << endl;
	TestProblem(FactorSpace(), FactorFunction(boost::bind(&Benchmarks::Cartesian<State>::Schwefel2_21, _1)));
	cout << "----- Cartesian Vec<double,31> --- Rosenbrock -----" << endl;
	TestProblem(FactorSpace(), FactorFunction(boost::bind(&Benchmarks::Cartesian<State>::Rosenbrock, _1)));
	cout << "----- Cartesian Vec<double,31> --- Rastrigin -----" << endl;
	TestProblem(FactorSpace(), FactorFunction(boost::bind(&Benchmarks::Cartesian<State>::Rastrigin, _1)));

	return 1;
}
