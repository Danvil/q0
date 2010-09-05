/*
 * main.cpp
 *
 *  Created on: Sep 4, 2010
 *      Author: david
 */

#include "Benchmarks/Cartesian.h"
#include "QuestZero/States/CState.h"
#include "QuestZero/SampleSet.h"
#include "QuestZero/Algorithms/RND.h"
#include "QuestZero/Algorithms/PSO.h"
#include <Danvil/Tools/Timer.h>
#include <iostream>
using std::cout;
using std::endl;

typedef TCState<double, 3> MyState;

class TestFunction
: public IFunction<MyState>
{
public:
	double operator()(const MyState& state) {
		return Benchmarks::Cartesian<MyState::V::ScalarType, MyState::V::Dimension>::Ackley(state.cartesian);
	}
};

struct MyTraits
{
	typedef MyState State;
	typedef TCStateOperator<double, 3> StateOperator;
	typedef TSample<MyTraits> Sample;
	typedef TSampleSet<MyTraits> SampleSet;
	typedef TCDomainBox<double, 3> Domain;
	typedef TestFunction Function;
};

template<typename ALGO, typename DOM, typename FNC>
void TestAlgo(ALGO algo, const DOM& dom, const FNC& fnc) {
	Danvil::Timer timer;
	timer.start();
	MyTraits::SampleSet best_many = algo.Optimize(dom, fnc);
	timer.stop();
	cout << "Time: " << timer.getElapsedTimeInMilliSec() << " ms" << endl;
	cout << "Result: " << best_many.best() << endl;
}

int main(int argc, char* argv[]) {
	cout << "Hello QuestZero!" << endl;

	MyTraits::Domain::V min(-3, -3, -3);
	MyTraits::Domain::V max(+3, +3, +3);
	PTR(MyTraits::Domain) dom(new MyTraits::Domain(min, max));

	PTR(MyTraits::Function) fnc(new MyTraits::Function());

	cout << "----- RND -----" << endl;
	RND<MyTraits> algoRnd;
	algoRnd.particleCount = 1000;
	algoRnd.maxIterations = 20;
	TestAlgo(algoRnd, dom, fnc);

	cout << "----- PSO -----" << endl;
	PSO<MyTraits> algoPso;
	algoPso.settings.particleCount = 1000;
	algoPso.settings.iterations = 20;
	TestAlgo(algoPso, dom, fnc);

	cout << "Bye QuestZero!" << endl;
	return 0;
}

#include "QuestZero/RandomNumbers.cpp"
