/*
 * Problem01.cpp
 *
 *  Created on: Sep 11, 2010
 *      Author: david
 */

#include "Test.h"
#include "Benchmarks/Cartesian.h"
#include "Benchmarks/PointCloudRegistration.h"
#include <QuestZero/Optimization.h>
#include <QuestZero/IFunction.h>
#include <QuestZero/Space/Cartesian.h>
#include <QuestZero/Space/SO3.h>
#include <QuestZero/Space/Mixed2.h>
#include <Danvil/Tools/Timer.h>
#include <Danvil/LinAlg.h>
#include <boost/bind.hpp>
#include <iostream>
using std::cout;
using std::endl;
//---------------------------------------------------------------------------

template<typename State>
double shiftedMinima(boost::function<double(const State&)> f, const State& shift, const State& x) {
	return f(x - shift);
}

//---------------------------------------------------------------------------

namespace Problem01
{
	typedef Danvil::ctLinAlg::Vec3f State;

	typedef FunctionFromDelegate<State> Function;

	PTR(Function) FactorFunction() {
		boost::function<double(const State&)> f1 = boost::bind(&Benchmarks::Cartesian<State>::Rastrigin, _1);
		State minima(0,1,2);
		return Danvil::Ptr(new Function(boost::bind(&shiftedMinima<State>, f1, minima, _1)));
	}

	typedef Spaces::Cartesian::CartesianSpace<State> Space;

	Space FactorSpace() {
		Space space;
		space.setDomainRange(State(3,4,5));
		return space;
	}

	void run() {
		cout << "----- Cartesian Vec3f -----" << endl;
		TestProblem(FactorSpace(), FactorFunction());
	}
}

//---------------------------------------------------------------------------

namespace Problem02
{
	typedef Danvil::ctLinAlg::Vec<double, 31> State;

	typedef FunctionFromDelegate<State> Function;

	PTR(Function) FactorFunction() {
		boost::function<double(const State&)> f1 = boost::bind(&Benchmarks::Cartesian<State>::Rastrigin, _1);
		State minima;
		for(unsigned int i=0; i<State::Dimension; i++) { minima[i] = 0;/*(double)i;*/ }
		return Danvil::Ptr(new Function(boost::bind(&shiftedMinima<State>, f1, minima, _1)));
	}

	typedef Spaces::Cartesian::CartesianSpace<State> Space;

	Space FactorSpace() {
		Space space;
		State range;
		for(unsigned int i=0; i<State::Dimension; i++) { range[i] = 10; }
		space.setDomainRange(range);
		return space;
	}

	void run() {
		cout << "----- Cartesian Vec<double,31> -----" << endl;
		TestProblem(FactorSpace(), FactorFunction());
	}
}

//---------------------------------------------------------------------------

namespace Problem03
{
	typedef Danvil::ctLinAlg::TQuaternion<double> State;

	class RegistrationFunction
	: public IFunction<State>,
	  public Benchmarks::PointCloudRegistration<double>
	{
	public:
		RegistrationFunction(size_t n)
		: Benchmarks::PointCloudRegistration<double>(n) {}

		double operator()(const State& state) {
			return fit(state.rotation());
		}
	};

	typedef RegistrationFunction Function;

	PTR(Function) FactorFunction() {
		return Danvil::Ptr(new RegistrationFunction(100));
	}

	typedef Spaces::SO3::SO3Space<double> Space;

	Space FactorSpace() { return Space(); }

	void run() {
		cout << "----- Registration -----" << endl;
		TestProblem(FactorSpace(), FactorFunction());
	}
}

//---------------------------------------------------------------------------

namespace Problem04
{
	typedef Danvil::ctLinAlg::Vec3<double> State1;
	typedef Spaces::Cartesian::CartesianSpace<State1> Space1;

	typedef Spaces::SO3::SO3Space<double> Space2;

	typedef Spaces::Mixed2State<Space1, 1, Space2> State;

	class RegistrationFunction
	: public IFunction<State>,
	  public Benchmarks::PointCloudRegistration<double>
	{
	public:
		RegistrationFunction(size_t n)
		: Benchmarks::PointCloudRegistration<double>(n) {}

		double operator()(const State& state) {
			return fit(state.sr[0].rotation(), state.sc);
		}
	};

	typedef RegistrationFunction Function;

	PTR(Function) FactorFunction() {
		return Danvil::Ptr(new RegistrationFunction(100));
	}

	typedef Spaces::Mixed2Space<Space1, 1, Space2> Space;

	Space FactorSpace() {
		Space1 space1;
		space1.setDomainRange(State1(3,4,5));
		Space2 space2;
		Space space;
		space.setSpace1(space1);
		space.setSpace2(0, space2);
		return space;
	}

	void run() {
		cout << "----- Registration w. position (Mixed2State) -----" << endl;
		TestProblem(FactorSpace(), FactorFunction());
	}
}


//---------------------------------------------------------------------------
