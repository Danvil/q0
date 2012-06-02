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
#include <boost/bind.hpp>
#include <iostream>

static const unsigned int N = 4;

typedef Eigen::Matrix<double,3,1> state_loc_t;
typedef Q0::Spaces::MultiplierState<state_loc_t, Q0::Spaces::MultiplierSizePolicies::FixedSize<N> > state_loc_multi_t;
typedef Eigen::Quaternion<double> state_rot_t;
typedef Q0::Spaces::MultiplierState<state_rot_t, Q0::Spaces::MultiplierSizePolicies::FixedSize<N> > state_rot_multi_t;
typedef Q0::Spaces::TypelistState<LOKI_TYPELIST_2(state_loc_multi_t,state_rot_multi_t)> state_t;

typedef Q0::Spaces::Cartesian::FiniteCartesianSpace<state_loc_t> space_loc_t;
typedef Q0::Spaces::MultiplierSpace<space_loc_t, state_loc_multi_t> space_loc_multi_t;
typedef Q0::Spaces::SO3::FullSO3Space<double> space_rot_t;
typedef Q0::Spaces::MultiplierSpace<space_rot_t, state_rot_multi_t> space_rot_multi_t;
typedef Q0::Spaces::TypelistSpace<LOKI_TYPELIST_2(space_loc_multi_t,space_rot_multi_t), state_t> space_t;

struct MultiRegistrationFunctionWPos
{
	typedef double Score;

	void createProblem(size_t n) {
		for(unsigned int i=0; i<N; i++) {
			r[i].reset(new Benchmarks::PointCloudRegistration<double>(N));
		}
	}

	Score operator()(const state_t& s) const {
		Score sum = 0;
		for(unsigned int i=0; i<N; i++) {
			Score x = r[i]->fit(s.part<1>()[i].toRotationMatrix(), s.part<0>()[i]);
			sum += x*x;
		}
		return sum;
	}

private:
	boost::shared_ptr<Benchmarks::PointCloudRegistration<double>> r[N];
};

int main(int argc, char** argv)
{
	std::cout << "----- Multi-Registration with position using TypelistSpace with Multiplier -----" << std::endl;

	space_t space;
	for(unsigned int i=0; i<N; i++) {
		space.space<0>()[i].setDomainRange(state_loc_t(3,4,5));
	}

	Q0::Functions::AddParallel<state_t,MultiRegistrationFunctionWPos> f;
	f.createProblem(25);

	TestProblem(space, f);
}
