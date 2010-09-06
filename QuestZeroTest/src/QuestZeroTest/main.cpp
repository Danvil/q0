/*
 * main.cpp
 *
 *  Created on: Sep 4, 2010
 *      Author: david
 */

#include "Benchmarks/Cartesian.h"
#include "Benchmarks/PointCloudRegistration.h"
#include "QuestZero/States/CState.h"
#include "QuestZero/States/RState.h"
#include "QuestZero/States/CRState.h"
#include "QuestZero/SampleSet.h"
#include "QuestZero/Algorithms/RND.h"
#include "QuestZero/Algorithms/PSO.h"
#include "QuestZero/Tracer/NoTracer.h"
#include <Danvil/Tools/Timer.h>
#include <iostream>
using std::cout;
using std::endl;

//---------------------------------------------------------------------------

typedef TCState<double, 3> MyState;

class TestFunction
: public IFunction<MyState>
{
public:
	double operator()(const MyState& state) {
		return Benchmarks::Cartesian<MyState::V::ScalarType, MyState::V::Dimension>::Sphere(state.cartesian);
	}
};

struct MyTraits
{
	typedef MyState State;
	typedef TCStateOperator<double, 3> StateOperator;
	typedef TCDomainBox<double, 3> Domain;
	typedef TestFunction Function;
	typedef NoTracer<State> Tracer;
};

//---------------------------------------------------------------------------

typedef TRState<double> RegistrationState;

class RegistrationFunction
: public IFunction<RegistrationState>,
  public Benchmarks::PointCloudRegistration<double>
{
public:
	RegistrationFunction(size_t n)
	: Benchmarks::PointCloudRegistration<double>(n) {}

	double operator()(const RegistrationState& state) {
		return fit(state.rot().rotation());
	}
};

struct RegistrationTraits
{
	typedef RegistrationState State;
	typedef TRStateOperator<double> StateOperator;
	typedef TRFull<double> Domain;
	typedef RegistrationFunction Function;
	typedef NoTracer<State> Tracer;
};

//---------------------------------------------------------------------------

typedef TCRState<double, 3, 1> RegistrationWPState;

class RegistrationWPFunction
: public IFunction<RegistrationWPState>,
  public Benchmarks::PointCloudRegistration<double>
{
public:
	RegistrationWPFunction(size_t n)
	: Benchmarks::PointCloudRegistration<double>(n) {}

	double operator()(const RegistrationWPState& state) {
		return fit(state.sr[0].rot().rotation(), Danvil::ctLinAlg::Vec3<double>::FactorFromPointer(state.sc.cartesian.begin()));
	}
};

struct RegistrationWPTraits
{
	typedef RegistrationWPState State;
	typedef TCRStateOperator<double, 3, 1> StateOperator;
	typedef TCRDomain<double, 3, 1> Domain;
	typedef RegistrationWPFunction Function;
	typedef NoTracer<State> Tracer;
};

//---------------------------------------------------------------------------

template<typename ALGO, typename Traits>
void TestAlgo(ALGO algo, const PTR(typename Traits::Domain)& dom, const PTR(typename Traits::Function)& fnc)
{
	Danvil::Timer timer;
	timer.start();
	TSampleSet<typename Traits::State> best_many = algo.Optimize2(dom, fnc);
	timer.stop();
	cout << "Time: " << timer.getElapsedTimeInMilliSec() << " ms" << endl;
	cout << "Result: " << best_many.best() << endl;
}

template<typename Traits>
void TestProblem(const PTR(typename Traits::Domain)& dom, const PTR(typename Traits::Function)& fnc)
{
	cout << "----- RND -----" << endl;
	RND<Traits> algoRnd;
	algoRnd.particleCount = 1000;
	algoRnd.maxIterations = 20;
	TestAlgo<RND<Traits>, Traits>(algoRnd, dom, fnc);

	cout << "----- PSO -----" << endl;
	PSO<Traits> algoPso;
	algoPso.settings.particleCount = 1000;
	algoPso.settings.iterations = 20;
	TestAlgo<PSO<Traits>, Traits>(algoPso, dom, fnc);
}

int main(int argc, char* argv[]) {
	cout << "Hello QuestZero!" << endl;
	cout << endl;

	cout << "----- Cartesian -----" << endl;
	MyTraits::Domain::V min(-3, -3, -3);
	MyTraits::Domain::V max(+3, +3, +3);
	PTR(MyTraits::Domain) dom(new MyTraits::Domain(min, max));
	PTR(MyTraits::Function) fnc(new MyTraits::Function());
	TestProblem<MyTraits>(dom, fnc);
	cout << endl;

	cout << "----- Registration -----" << endl;
	PTR(RegistrationTraits::Domain) r_dom(new RegistrationTraits::Domain());
	PTR(RegistrationTraits::Function) r_fnc(new RegistrationTraits::Function(100));
	TestProblem<RegistrationTraits>(r_dom, r_fnc);
	cout << endl;

	cout << "----- Registration w. position -----" << endl;
	PTR(RegistrationWPTraits::Domain) rwp_dom(new RegistrationWPTraits::Domain(min, max));
	PTR(RegistrationWPTraits::Function) rwp_fnc(new RegistrationWPTraits::Function(100));
	TestProblem<RegistrationWPTraits>(rwp_dom, rwp_fnc);
	cout << endl;

	cout << "Bye QuestZero!" << endl;
	return 0;
}

//---------------------------------------------------------------------------

#include "QuestZero/RandomNumbers.cpp"

//---------------------------------------------------------------------------
