/*
 * PointCloudRegistration_LocRot.cpp
 *
 *  Created on: May 30, 2012
 *      Author: david
 */

#include "../Test.h"
#include "../Benchmarks/PointCloudRegistration.h"
#include <QuestZero/Optimization/Optimization.h>
#include <QuestZero/Optimization/Functions.h>
#include <QuestZero/Spaces/Cartesian.h>
#include <QuestZero/Spaces/SO3.h>
#include <QuestZero/Spaces/TypelistSpace.h>
#include <Danvil/LinAlg.h>
#include <Danvil/Print.h>
#include <boost/bind.hpp>
#include <iostream>
using std::cout;
using std::endl;
using namespace Q0;

typedef double Real;

typedef Danvil::ctLinAlg::Vec3<Real> state_loc_t;
typedef Danvil::SO3::Quaternion<Real> state_rot_t;
typedef Spaces::TypelistState<LOKI_TYPELIST_2(state_loc_t,state_rot_t)> state_t;

typedef Spaces::Cartesian::FiniteCartesianSpace<state_loc_t> space_loc_t;
typedef Spaces::SO3::FullSO3Space<Real> space_rot_t;
typedef Spaces::TypelistSpace<LOKI_TYPELIST_2(space_loc_t,space_rot_t), state_t> space_t;

struct RegistrationFunction
: public Benchmarks::PointCloudRegistration<Real>
{
	typedef double Score;
	Real operator()(const state_t& x) const {
		return fit(Danvil::SO3::ConvertToMatrix(x.part<1>()), x.part<0>());
	}
};

int main(int argc, char* argv[])
{
	cout << "----- Registration (rotation and location) (TypelistSpace) -----" << endl;

	space_t space;
	space.space<0>().setDomainRange(state_loc_t(3,4,5));

	Functions::AddParallel<state_t, RegistrationFunction> f;
	f.createRandomProblem(100);

	TestProblem(space, f);

	return 1;
}
