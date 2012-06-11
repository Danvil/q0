/*
 * PointCloudRegistration_Rot.cpp
 *
 *  Created on: May 30, 2012
 *      Author: david
 */

#include "Solve.h"
#include "PointCloudRegistration.h"
#include <QuestZero/Optimization/Optimization.h>
#include <QuestZero/Spaces/SO3.h>
#include <boost/bind.hpp>
#include <iostream>

typedef double real_t;

typedef Eigen::Quaternion<real_t> state_t;

typedef Q0::Spaces::FullSO3Space<real_t> space_t;

struct RegistrationFunction
: public Benchmarks::PointCloudRegistration<real_t>
{
	typedef double Score;
	double operator()(const state_t& x) const {
		return fit(x.toRotationMatrix());
	}
};

int main(int argc, char* argv[])
{
	std::cout << "----- Registration (rotation only) -----" << std::endl;

	space_t space;

	RegistrationFunction f;
	f.createRandomProblem(100);

	Solve(space, f, 100, true);

	return 1;
}
