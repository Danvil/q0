/*
 * Problem06.cpp
 *
 *  Created on: Sep 11, 2010
 *      Author: david
 */

#include "Test.h"
#include "Benchmarks/PointCloudRegistration.h"
#include <QuestZero/Spaces/SO3.h>
#include <QuestZero/Spaces/Multiplier.h>
#include <QuestZero/Optimization/Functions.h>
#include <Danvil/LinAlg.h>
#include <Danvil/Tools/MoreMath.h>
#include <boost/bind.hpp>
#include <iostream>
using std::cout;
using std::endl;
using namespace Q0;

namespace Problem06
{
	static const unsigned int N = 4;

	typedef Danvil::SO3::Quaternion<double> base_state;
	typedef Spaces::MultiplierState<base_state, Spaces::MultiplierSizePolicies::FixedSize<N> > state;

	typedef Spaces::SO3::SO3Space<double> base_space;
	typedef Spaces::MultiplierSpace<base_space, state> space;

	space FactorSpace() {
		return space();
	}

	struct MultiRegistrationFunction
	{
		typedef double Score;
		void createProblem(size_t n) {
			for(unsigned int i=0; i<N; i++) {
				r[i] = Danvil::Ptr(new Benchmarks::PointCloudRegistration<double>(N));
			}
		}

		Score operator()(const state& s) const {
			Score sum = 0;
			for(unsigned int i=0; i<N; i++) {
				Score x = r[i]->fit(Danvil::SO3::ConvertToMatrix(s[i]));
				sum += x*x;
			}
			return sum;
		}

	private:
		PTR(Benchmarks::PointCloudRegistration<double>) r[N];
	};

	typedef Functions::AddParallel<state, MultiRegistrationFunction> function;

	function FactorFunction() {
		function f;
		f.createProblem(25);
		return f;
	}

	void run()
	{
		cout << "----- Multi-Registration with Multiplier State -----" << endl;
		TestProblem(FactorSpace(), FactorFunction());
	}
}
