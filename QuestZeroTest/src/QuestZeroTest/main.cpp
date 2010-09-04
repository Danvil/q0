/*
 * main.cpp
 *
 *  Created on: Sep 4, 2010
 *      Author: david
 */

#include "QuestZero/State.h"
#include "QuestZero/SampleSet.h"
#include "QuestZero/Algorithms/RND.h"
#include <iostream>
using std::cout;
using std::endl;

struct Traits
{
	typedef TState<double, 3, 0> State;
	typedef TSample<Traits> Sample;
	typedef TSampleSet<Traits> SampleSet;
	typedef IFunction<Traits> Function;
};

class TestFunction
: public IFunction<Traits>
{
public:
	double operator()(const Traits::State& state) {
		return 0.0;
	}
};

int main(int argc, char* argv[]) {
	cout << "Hello QuestZero!" << endl;
	PTR(TestFunction) f(new TestFunction());
	RND<Traits> rnd;
	Traits::SampleSet best = rnd.Optimize(f);
	cout << "Bye QuestZero!" << endl;
	return 0;
}
