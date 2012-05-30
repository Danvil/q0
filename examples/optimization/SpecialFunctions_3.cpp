/*
 * SpecialFunctions_3.cpp
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

typedef Danvil::ctLinAlg::Vec3f State;

typedef Spaces::Cartesian::FiniteCartesianSpace<State> Space;

Space FactorSpace() {
	Space space;
	space.setDomainRange(State(3,4,5));
	return space;
}

typedef Functions::BoostFunctionSingleWrapper<State,double> Function;

template<typename State>
double shiftedMinima(boost::function<double(const State&)> f, const State& shift, const State& x) {
	return f(x - shift);
}

Function FactorFunction(boost::function<double(const State&)> f1) {
	Function f;
	State minima(0,1,2);
	f.set_functor(boost::bind(&shiftedMinima<State>, f1, minima, _1));
	return f;
}

int main(int argc, char* argv[])
{
	cout << "----- Cartesian Vec3f --- DiscreetSphere -----" << endl;
	TestProblem(FactorSpace(), FactorFunction(boost::bind(&Benchmarks::Cartesian<State>::DiscreetSphere, _1)));
	cout << "----- Cartesian Vec3f --- Schwefel2_21 -----" << endl;
	TestProblem(FactorSpace(), FactorFunction(boost::bind(&Benchmarks::Cartesian<State>::Schwefel2_21, _1)));
	cout << "----- Cartesian Vec3f --- Rosenbrock -----" << endl;
	TestProblem(FactorSpace(), FactorFunction(boost::bind(&Benchmarks::Cartesian<State>::Rosenbrock, _1)));
	cout << "----- Cartesian Vec3f --- Rastrigin -----" << endl;
	TestProblem(FactorSpace(), FactorFunction(boost::bind(&Benchmarks::Cartesian<State>::Rastrigin, _1)));

	return 1;
}
