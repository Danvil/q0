/*
 * main.cpp
 *
 *  Created on: Sep 4, 2010
 *      Author: david
 */

#include "QuestZero/States/CState.h"
#include "QuestZero/SampleSet.h"
#include "QuestZero/Algorithms/RND.h"
#include <iostream>
using std::cout;
using std::endl;

typedef TCState<double, 3> MyState;

class TestFunction
: public IFunction<MyState>
{
public:
	double operator()(const MyState& state) {
		return Danvil::ctLinAlg::Norm(state.cartesian);
	}
};

struct MyTraits
{
	typedef MyState State;
	typedef TSample<MyTraits> Sample;
	typedef TSampleSet<MyTraits> SampleSet;
	typedef TCDomainBox<double, 3> Domain;
	typedef TestFunction Function;
};

int main(int argc, char* argv[]) {
	cout << "Hello QuestZero!" << endl;

	MyTraits::Domain::V min(-3, -3, -3);
	MyTraits::Domain::V max(+3, +3, +3);
	PTR(MyTraits::Domain) dom(new MyTraits::Domain(min, max));

	PTR(MyTraits::Function) fnc(new MyTraits::Function());

	RND<MyTraits> rnd;

	MyTraits::SampleSet best_many = rnd.Optimize(dom, fnc);

	cout << "----- RESULT ----" << endl;
	cout << best_many.best() << endl;
	cout << "----- RESULT ----" << endl;

	cout << "Bye QuestZero!" << endl;
	return 0;
}

#include "QuestZero/RandomNumbers.cpp"
