/*
 * main.cpp
 *
 *  Created on: Sep 4, 2010
 *      Author: david
 */

#include "Benchmarks/Cartesian.h"
#include "Benchmarks/PointCloudRegistration.h"
#include <QuestZero/Optimization.h>
#include <QuestZero/IFunction.h>
#include <QuestZero/Tracer/ProgressAndBestToConsoleTracer.h>
#include <QuestZero/Algorithms/RND.h>
#include <QuestZero/Algorithms/PSO.h>
#include <QuestZero/Space/Cartesian.h>
#include <QuestZero/Space/SO3.h>
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
}

//---------------------------------------------------------------------------

//typedef TRState<double> RegistrationState;
//
//class RegistrationFunction
//: public IFunction<RegistrationState>,
//  public Benchmarks::PointCloudRegistration<double>
//{
//public:
//	RegistrationFunction(size_t n)
//	: Benchmarks::PointCloudRegistration<double>(n) {}
//
//	double operator()(const RegistrationState& state) {
//		return fit(state.rot().rotation());
//	}
//};
//
//struct RegistrationTraits
//{
//	typedef RegistrationState State;
//	typedef TRStateOperator<double> StateOperator;
//	typedef TRFull<double> Domain;
//	typedef RegistrationFunction Function;
//	typedef NoTracer<State> Tracer;
//};

//---------------------------------------------------------------------------

//typedef TCRState<double, 3, 1> RegistrationWPState;
//
//class RegistrationWPFunction
//: public IFunction<RegistrationWPState>,
//  public Benchmarks::PointCloudRegistration<double>
//{
//public:
//	RegistrationWPFunction(size_t n)
//	: Benchmarks::PointCloudRegistration<double>(n) {}
//
//	double operator()(const RegistrationWPState& state) {
//		return fit(state.sr[0].rot().rotation(), Danvil::ctLinAlg::Vec3<double>::FactorFromPointer(state.sc.cartesian.begin()));
//	}
//};
//
//struct RegistrationWPTraits
//{
//	typedef RegistrationWPState State;
//	typedef TCRStateOperator<double, 3, 1> StateOperator;
//	typedef TCRDomain<double, 3, 1> Domain;
//	typedef RegistrationWPFunction Function;
//	typedef NoTracer<State> Tracer;
//};

//---------------------------------------------------------------------------

template<typename ALGO, class Space, class Function>
void TestAlgo(ALGO algo, const Space& space, const PTR(Function)& function)
{
	Danvil::Timer timer;
	timer.start();
	TSample<typename ALGO::State> best = algo.optimize(space, function);
	timer.stop();
	cout << "Time: " << timer.getElapsedTimeInMilliSec() << " ms" << endl;
	cout << "Result: " << best << endl;
}

#ifdef DEBUG
	template<typename State> struct MyTracer : ProgressAndBestToConsoleTracer<State> {};
#else
	template<typename State> struct MyTracer : NoTracer<State> {};
#endif

template<class Space, class Function>
void TestProblem(const Space& space, const PTR(Function)& function)
{
	cout << "----- RND -----" << endl;
	Optimization<typename Space::State, RND, StartingStatePicker::RandomPicker, TakePolicy::TakeBest, MyTracer> algoRnd;
	algoRnd.particleCount = 1000;
	algoRnd.maxIterations = 20;
	TestAlgo(algoRnd, space, function);

	cout << "----- PSO -----" << endl;
	Optimization<typename Space::State, PSO, StartingStatePicker::RandomPicker, TakePolicy::TakeBest, MyTracer> algoPso;
	algoPso.settings.particleCount = 1000;
	algoPso.settings.iterations = 20;
	TestAlgo(algoPso, space, function);

	cout << endl;
}

int main(int argc, char* argv[]) {
	cout << "Hello QuestZero!" << endl;
	cout << endl;

	cout << "----- Cartesian Vec3f -----" << endl;
	TestProblem(Problem01::FactorSpace(), Problem01::FactorFunction());

	cout << "----- Cartesian Vec<double,31> -----" << endl;
	TestProblem(Problem02::FactorSpace(), Problem02::FactorFunction());

	cout << "----- Registration -----" << endl;
	TestProblem(Problem03::FactorSpace(), Problem03::FactorFunction());

//	cout << "----- Registration -----" << endl;
//	PTR(RegistrationTraits::Domain) r_dom(new RegistrationTraits::Domain());
//	PTR(RegistrationTraits::Function) r_fnc(new RegistrationTraits::Function(100));
//	TestProblem<RegistrationTraits>(r_dom, r_fnc);
//	cout << endl;

//	cout << "----- Registration w. position -----" << endl;
//	PTR(RegistrationWPTraits::Domain) rwp_dom(new RegistrationWPTraits::Domain(min, max));
//	PTR(RegistrationWPTraits::Function) rwp_fnc(new RegistrationWPTraits::Function(100));
//	TestProblem<RegistrationWPTraits>(rwp_dom, rwp_fnc);
//	cout << endl;

	cout << "Bye QuestZero!" << endl;
	return 0;
}

//---------------------------------------------------------------------------

#include "QuestZero/RandomNumbers.cpp"

//---------------------------------------------------------------------------
