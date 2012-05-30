/*
 * PointCloudRegistration_Rot_Multi.cpp
 *
 *  Created on: Sep 11, 2010
 *      Author: david
 */

#include "../Test.h"
#include "../Benchmarks/PointCloudRegistration.h"
#include <QuestZero/Spaces/SO3.h>
#include <QuestZero/Spaces/Multiplier.h>
#include <QuestZero/Optimization/Functions.h>
#include <Danvil/LinAlg.h>
#include <Danvil/Print.h>
#include <Danvil/Tools/MoreMath.h>
#include <boost/bind.hpp>
#include <iostream>
using std::cout;
using std::endl;
using namespace Q0;

static const unsigned int N = 4;

typedef Danvil::SO3::Quaternion<double> state_rot_t;
typedef Spaces::MultiplierState<state_rot_t, Spaces::MultiplierSizePolicies::FixedSize<N> > state_t;

typedef Spaces::SO3::FullSO3Space<double> space_rot_t;
typedef Spaces::MultiplierSpace<space_rot_t, state_t> space_t;

struct MultiRegistrationFunction
{
	typedef double Score;
	void createProblem(size_t n) {
		for(unsigned int i=0; i<N; i++) {
			r[i] = Danvil::Ptr(new Benchmarks::PointCloudRegistration<double>(N));
		}
	}

	Score operator()(const state_t& s) const {
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

typedef Functions::AddParallel<state_t, MultiRegistrationFunction> function;

function FactorFunction() {
}

int main(int argc, char** argv)
{
	cout << "----- Multi-Registration with Multiplier State -----" << endl;

	space_t space;

	function f;
	f.createProblem(25);

	TestProblem(space, f);

	return 1;
}
