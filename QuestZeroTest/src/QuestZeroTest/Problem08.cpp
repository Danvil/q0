/*
 * Problem08.cpp
 *
 *  Created on: Sep 11, 2010
 *      Author: david
 */

#include "Test.h"
#include "Benchmarks/PointCloudRegistration.h"
#include <QuestZero/Spaces/Cartesian.h>
#include <QuestZero/Spaces/SO3.h>
#include <QuestZero/Spaces/TypelistSpace.h>
#include <QuestZero/Spaces/Multiplier.h>
#include <QuestZero/Optimization/Functions.h>
#include <Danvil/LinAlg.h>
#include <Danvil/SO3.h>
#include <boost/bind.hpp>
#include <iostream>
using std::cout;
using std::endl;
using namespace Q0;

namespace Problem08
{
	static const unsigned int N = 4;

	typedef Danvil::ctLinAlg::Vec3<double> base_state_0;
	typedef Spaces::MultiplierState<base_state_0, Spaces::MultiplierSizePolicies::FixedSize<N> > state0;
	typedef Danvil::SO3::Quaternion<double> base_state_1;
	typedef Spaces::MultiplierState<base_state_1, Spaces::MultiplierSizePolicies::FixedSize<N> > state1;
	typedef LOKI_TYPELIST_2(state0,state1) state_types;
	typedef Spaces::TypelistState<state_types> state;

	typedef Spaces::Cartesian::FiniteCartesianSpace<base_state_0> base_space_0;
	typedef Spaces::MultiplierSpace<base_space_0, state0> space0;
	typedef Spaces::SO3::SO3Space<double> base_space_1;
	typedef Spaces::MultiplierSpace<base_space_1, state1> space1;
	typedef LOKI_TYPELIST_2(space0,space1) space_types;
	typedef Spaces::TypelistSpace<space_types, state> space;

	space FactorSpace() {
		space myspace;
		for(unsigned int i=0; i<N; i++) {
			myspace.space<0>()[i].setDomainRange(base_state_0(3,4,5));
		}
		return myspace;
	}

	struct MultiRegistrationFunctionWPos
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
				Score x = r[i]->fit(Danvil::SO3::ConvertToMatrix(s.part<1>()[i]), s.part<0>()[i]);
				sum += x*x;
			}
			return sum;
		}

	private:
		PTR(Benchmarks::PointCloudRegistration<double>) r[N];
	};

	typedef Functions::AddParallel<state,MultiRegistrationFunctionWPos> function;

	function FactorFunction() {
		function f;
		f.createProblem(25);
		return f;
	}

	void run()
	{
		cout << "----- Multi-Registration with position using TypelistSpace with Multiplier -----" << endl;
		TestProblem(FactorSpace(), FactorFunction());
	}
}
