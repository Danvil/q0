/*
 * Problem08.cpp
 *
 *  Created on: Sep 11, 2010
 *      Author: david
 */

#include "Test.h"
#include "Benchmarks/PointCloudRegistration.h"
#include <QuestZero/Space/Cartesian.h>
#include <QuestZero/Space/SO3.h>
#include <QuestZero/Space/TypelistSpace.h>
#include <QuestZero/Space/Multiplier.h>
#include <QuestZero/IFunction.h>
#include <Danvil/LinAlg.h>
#include <Danvil/Tools/Small.h>
#include <boost/bind.hpp>
#include <iostream>
using std::cout;
using std::endl;

namespace Problem08
{
	static const unsigned int N = 4;

	typedef Danvil::ctLinAlg::Vec3<double> base_state_0;
	typedef Spaces::MultiplierState<base_state_0, N> state0;
	typedef Danvil::ctLinAlg::TQuaternion<double> base_state_1;
	typedef Spaces::MultiplierState<base_state_1, N> state1;
	typedef LOKI_TYPELIST_2(state0,state1) state_types;
	typedef Spaces::TypelistState<state_types> state;

	typedef Spaces::Cartesian::CartesianSpace<base_state_0> base_space_0;
	typedef Spaces::MultiplierSpace<base_space_0, state0> space0;
	typedef Spaces::SO3::SO3Space<double> base_space_1;
	typedef Spaces::MultiplierSpace<base_space_1, state1> space1;
	typedef LOKI_TYPELIST_2(space0,space1) space_types;
	typedef Spaces::TypelistSpace<space_types, state> space;

	class MultiRegistrationFunctionWPos
	: public IFunction<state>
	{
	public:
		MultiRegistrationFunctionWPos(size_t n) {
			for(unsigned int i=0; i<N; i++) {
				r[i] = Danvil::Ptr(new Benchmarks::PointCloudRegistration<double>(N));
			}
		}

		double operator()(const state& s) {
			double sum = 0;
			for(unsigned int i=0; i<N; i++) {
				double x = r[i]->fit(s.part<1>()[i].rotation(), s.part<0>()[i]);
				sum += x*x;
			}
			return sum;
		}

	private:
		PTR(Benchmarks::PointCloudRegistration<double>) r[N];
	};

	typedef MultiRegistrationFunctionWPos function;

	space FactorSpace() {
		space myspace;
		for(unsigned int i=0; i<N; i++) {
			myspace.space<0>()[i].setDomainRange(base_state_0(3,4,5));
		}
		return myspace;
	}

	PTR(function) FactorFunction() {
		return Danvil::Ptr(new function(25));
	}

	void run()
	{
		cout << "----- Multi-Registration with position using TypelistSpace with Multiplier -----" << endl;
		TestProblem(FactorSpace(), FactorFunction());
	}
}
