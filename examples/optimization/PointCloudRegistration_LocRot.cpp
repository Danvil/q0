/*
 * PointCloudRegistration_LocRot.cpp
 *
 *  Created on: May 30, 2012
 *      Author: david
 */

#include "Solve.h"
#include "PointCloudRegistration.h"
#include <QuestZero/Optimization/Optimization.h>
#include <QuestZero/Optimization/Functions.h>
#include <QuestZero/Spaces/Cartesian.h>
#include <QuestZero/Spaces/SO3.h>
#include <QuestZero/Spaces/TypelistSpace.h>
#include <boost/bind.hpp>
#include <iostream>

typedef double Real;

typedef Eigen::Matrix<Real,3,1> state_loc_t;
typedef Eigen::Quaternion<Real> state_rot_t;
typedef Q0::Spaces::TypelistState<LOKI_TYPELIST_2(state_loc_t,state_rot_t)> state_t;

typedef Q0::Spaces::FiniteCartesianSpace<state_loc_t> space_loc_t;
typedef Q0::Spaces::FullSO3Space<Real> space_rot_t;
typedef Q0::Spaces::TypelistSpace<LOKI_TYPELIST_2(space_loc_t,space_rot_t), state_t> space_t;

struct RegistrationFunction
: public Benchmarks::PointCloudRegistration<Real>
{
	typedef double Score;
	Real operator()(const state_t& x) const {
		return fit(x.part<1>().toRotationMatrix(), x.part<0>());
	}
};

int main(int argc, char* argv[])
{

	unsigned int p_num_particles = 10000;
	bool p_verbose = true;

	std::cout << "----- Registration (rotation and location) (TypelistSpace) -----" << std::endl;

	space_t space;
	space.space<0>().setDomainRange(state_loc_t(3,4,5));

	Q0::Functions::AddParallel<state_t, RegistrationFunction> f;
	f.createRandomProblem(100);

	TestProblem(space, f, p_num_particles, p_verbose);

	return 1;
}
