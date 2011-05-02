/*
 * Problem01.cpp
 *
 *  Created on: Sep 11, 2010
 *      Author: david
 */

#include "Test.h"
#include "Benchmarks/Cartesian.h"
#include "Benchmarks/PointCloudRegistration.h"
#include <QuestZero/Optimization/Optimization.h>
#include <QuestZero/Optimization/Functions.h>
#include <QuestZero/Spaces/Cartesian.h>
#include <QuestZero/Spaces/SO3.h>
#include <QuestZero/Spaces/Mixed2.h>
#include <Danvil/Tools/Timer.h>
#include <Danvil/LinAlg.h>
#include <boost/bind.hpp>
#include <iostream>
using std::cout;
using std::endl;
using namespace Q0;
//---------------------------------------------------------------------------

template<typename State>
double shiftedMinima(boost::function<double(const State&)> f, const State& shift, const State& x) {
	return f(x - shift);
}

//---------------------------------------------------------------------------

namespace Problem01
{
	typedef Danvil::ctLinAlg::Vec3f State;

	typedef Spaces::Cartesian::FiniteCartesianSpace<State> Space;

	Space FactorSpace() {
		Space space;
		space.setDomainRange(State(3,4,5));
		return space;
	}

	typedef Functions::BoostFunctionSingleWrapper<State,double> Function;

	Function FactorFunction(boost::function<double(const State&)> f1) {
		Function f;
		State minima(0,1,2);
		f.set_functor(boost::bind(&shiftedMinima<State>, f1, minima, _1));
		return f;
	}

	void run() {
		cout << "----- Cartesian Vec3f --- DiscreetSphere -----" << endl;
		TestProblem(FactorSpace(), FactorFunction(boost::bind(&Benchmarks::Cartesian<State>::DiscreetSphere, _1)));
		cout << "----- Cartesian Vec3f --- Schwefel2_21 -----" << endl;
		TestProblem(FactorSpace(), FactorFunction(boost::bind(&Benchmarks::Cartesian<State>::Schwefel2_21, _1)));
		cout << "----- Cartesian Vec3f --- Rosenbrock -----" << endl;
		TestProblem(FactorSpace(), FactorFunction(boost::bind(&Benchmarks::Cartesian<State>::Rosenbrock, _1)));
		cout << "----- Cartesian Vec3f --- Rastrigin -----" << endl;
		TestProblem(FactorSpace(), FactorFunction(boost::bind(&Benchmarks::Cartesian<State>::Rastrigin, _1)));
	}
}

//---------------------------------------------------------------------------

namespace Problem02
{
	typedef Danvil::ctLinAlg::Vec<double, 31> State;

	typedef Spaces::Cartesian::FiniteCartesianSpace<State> Space;

	Space FactorSpace() {
		Space space;
		State range;
		for(unsigned int i=0; i<State::Dimension; i++) { range[i] = 10; }
		space.setDomainRange(range);
		return space;
	}

	typedef Functions::BoostFunctionSingleWrapper<State,double> Function;

	Function FactorFunction(boost::function<double(const State&)> f1) {
		Function f;
		State minima;
		for(unsigned int i=0; i<State::Dimension; i++) { minima[i] = 0.1 * (double)i; }
		f.set_functor(boost::bind(&shiftedMinima<State>, f1, minima, _1));
		return f;
	}

	void run() {
		cout << "----- Cartesian Vec<double,31> --- DiscreetSphere -----" << endl;
		TestProblem(FactorSpace(), FactorFunction(boost::bind(&Benchmarks::Cartesian<State>::DiscreetSphere, _1)));
		cout << "----- Cartesian Vec<double,31> --- Schwefel2_21 -----" << endl;
		TestProblem(FactorSpace(), FactorFunction(boost::bind(&Benchmarks::Cartesian<State>::Schwefel2_21, _1)));
		cout << "----- Cartesian Vec<double,31> --- Rosenbrock -----" << endl;
		TestProblem(FactorSpace(), FactorFunction(boost::bind(&Benchmarks::Cartesian<State>::Rosenbrock, _1)));
		cout << "----- Cartesian Vec<double,31> --- Rastrigin -----" << endl;
		TestProblem(FactorSpace(), FactorFunction(boost::bind(&Benchmarks::Cartesian<State>::Rastrigin, _1)));
	}
}

//---------------------------------------------------------------------------

namespace Problem03
{
	typedef Danvil::SO3::Quaternion<double> state;

	typedef Spaces::SO3::SO3Space<double> Space;

	Space FactorSpace() { return Space(); }

	struct RegistrationFunction
	: public Benchmarks::PointCloudRegistration<double>
	{
		typedef double Score;
		double operator()(const state& x) const {
			return fit(Danvil::SO3::ConvertToMatrix(x));
		}
	};

	typedef Functions::AddParallel<state,RegistrationFunction> function;

	function FactorFunction() {
		function f;
		f.createRandomProblem(100);
		return f;
	}

	void run() {
		cout << "----- Registration -----" << endl;
		TestProblem(FactorSpace(), FactorFunction());
	}
}

////---------------------------------------------------------------------------
//
//namespace Problem04
//{
//	typedef Danvil::ctLinAlg::Vec3<double> State1;
//	typedef Spaces::Cartesian::FiniteCartesianSpace<State1> Space1;
//
//	typedef Spaces::SO3::SO3Space<double> Space2;
//
//	typedef Spaces::Mixed2State<Space1, 1, Space2> state;
//
//	typedef Spaces::Mixed2Space<Space1, 1, Space2> space;
//
//	space FactorSpace() {
//		Space1 space1;
//		space1.setDomainRange(State1(3,4,5));
//		Space2 space2;
//		space s;
//		s.setSpace1(space1);
//		s.setSpace2(0, space2);
//		return s;
//	}
//
//	struct RegistrationFunction
//	: public Benchmarks::PointCloudRegistration<double>
//	{
//		typedef double Score;
//		double operator()(const state& x) const {
//			return fit(Danvil::SO3::ConvertToMatrix(x.sr[0]), x.sc);
//		}
//	};
//
//	typedef Functions::AddParallel<state, RegistrationFunction> function;
//
//	function FactorFunction() {
//		function f;
//		f.createRandomProblem(100);
//		return f;
//	}
//
//	void run() {
//		cout << "----- Registration w. position (Mixed2State) -----" << endl;
//		TestProblem(FactorSpace(), FactorFunction());
//	}
//}
//

//---------------------------------------------------------------------------
