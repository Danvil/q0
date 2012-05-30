/*
 * PointCloudRegistration_LocRot_Multi.cpp
 *
 *  Created on: Sep 11, 2010
 *      Author: david
 */

#include "../Test.h"
#include "../Benchmarks/PointCloudRegistration.h"
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

static const unsigned int N = 4;

typedef Danvil::ctLinAlg::Vec3<double> state_loc_t;
typedef Spaces::MultiplierState<state_loc_t, Spaces::MultiplierSizePolicies::FixedSize<N> > state_loc_multi_t;
typedef Danvil::SO3::Quaternion<double> state_rot_t;
typedef Spaces::MultiplierState<state_rot_t, Spaces::MultiplierSizePolicies::FixedSize<N> > state_rot_multi_t;
typedef Spaces::TypelistState<LOKI_TYPELIST_2(state_loc_multi_t,state_rot_multi_t)> state_t;

typedef Spaces::Cartesian::FiniteCartesianSpace<state_loc_t> space_loc_t;
typedef Spaces::MultiplierSpace<space_loc_t, state_loc_multi_t> space_loc_multi_t;
typedef Spaces::SO3::FullSO3Space<double> space_rot_t;
typedef Spaces::MultiplierSpace<space_rot_t, state_rot_multi_t> space_rot_multi_t;
typedef Spaces::TypelistSpace<LOKI_TYPELIST_2(space_loc_multi_t,space_rot_multi_t), state_t> space_t;

struct MultiRegistrationFunctionWPos
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
			Score x = r[i]->fit(Danvil::SO3::ConvertToMatrix(s.part<1>()[i]), s.part<0>()[i]);
			sum += x*x;
		}
		return sum;
	}

private:
	PTR(Benchmarks::PointCloudRegistration<double>) r[N];
};

int main(int argc, char** argv)
{
	cout << "----- Multi-Registration with position using TypelistSpace with Multiplier -----" << endl;

	space_t space;
	for(unsigned int i=0; i<N; i++) {
		space.space<0>()[i].setDomainRange(state_loc_t(3,4,5));
	}

	Functions::AddParallel<state_t,MultiRegistrationFunctionWPos> f;
	f.createProblem(25);

	TestProblem(space, f);
}
