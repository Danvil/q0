/*
 * PointCloudRegistration_Rot.cpp
 *
 *  Created on: May 30, 2012
 *      Author: david
 */

#include "../Test.h"
#include "../Benchmarks/PointCloudRegistration.h"
#include <QuestZero/Optimization/Optimization.h>
#include <QuestZero/Optimization/Functions.h>
#include <QuestZero/Spaces/SO3.h>
#include <Danvil/LinAlg.h>
#include <boost/bind.hpp>
#include <iostream>
using std::cout;
using std::endl;
using namespace Q0;

typedef Danvil::SO3::Quaternion<double> state_t;

typedef Spaces::SO3::FullSO3Space<double> space_t;

struct RegistrationFunction
: public Benchmarks::PointCloudRegistration<double>
{
	typedef double Score;
	double operator()(const state_t& x) const {
		return fit(Danvil::SO3::ConvertToMatrix(x));
	}
};

int main(int argc, char* argv[])
{
	cout << "----- Registration (rotation only) -----" << endl;

	space_t space;

	Functions::AddParallel<state_t,RegistrationFunction> f;
	f.createRandomProblem(100);

	TestProblem(space, f);

	return 1;
}
