/*
 * Problem06.cpp
 *
 *  Created on: Sep 11, 2010
 *      Author: david
 */

#include "Test.h"
#include "Benchmarks/PointCloudRegistration.h"
#include <QuestZero/Space/SO3.h>
#include <QuestZero/Space/Multiplier.h>
#include <QuestZero/IFunction.h>
#include <Danvil/LinAlg.h>
#include <Danvil/Tools/Small.h>
#include <boost/bind.hpp>
#include <iostream>
using std::cout;
using std::endl;

namespace Problem06
{
	static const unsigned int N = 4;

	typedef Danvil::ctLinAlg::TQuaternion<double> base_state;
	typedef Spaces::MultiplierState<base_state, N> state;

	typedef Spaces::SO3::SO3Space<double> base_space;
	typedef Spaces::MultiplierSpace<base_space, state> space;

	space FactorSpace() {
		return space();
	}

	class MultiRegistrationFunction
	: public IFunction<state>
	{
	public:
		MultiRegistrationFunction(size_t n) {
			for(unsigned int i=0; i<N; i++) {
				r[i] = Danvil::Ptr(new Benchmarks::PointCloudRegistration<double>(N));
			}
		}

		double operator()(const state& s) {
			double sum = 0;
			for(unsigned int i=0; i<N; i++) {
				double x = r[i]->fit(s[i].rotation());
				sum += x*x;
			}
			return sum;
		}

	private:
		PTR(Benchmarks::PointCloudRegistration<double>) r[N];
	};

	typedef MultiRegistrationFunction function;

	PTR(function) FactorFunction() {
		return Danvil::Ptr(new function(25));
	}

	void run()
	{
		cout << "----- Multi-Registration with Multiplier State -----" << endl;
		TestProblem(FactorSpace(), FactorFunction());
	}
}
