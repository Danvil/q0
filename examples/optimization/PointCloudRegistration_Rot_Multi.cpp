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
#include <QuestZero/Spaces/SO3.h>
#include <boost/bind.hpp>
#include <iostream>

static const unsigned int N = 4;

typedef Eigen::Quaternion<double> state_rot_t;
typedef Spaces::MultiplierState<state_rot_t, Spaces::MultiplierSizePolicies::FixedSize<N> > state_t;

typedef Spaces::SO3::FullSO3Space<double> space_rot_t;
typedef Spaces::MultiplierSpace<space_rot_t, state_t> space_t;

struct MultiRegistrationFunction
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
			Score x = r[i]->fit(s[i].toRotationMatrix());
			sum += x*x;
		}
		return sum;
	}

private:
	boost::shared_ptr<Benchmarks::PointCloudRegistration<double>> r[N];
};

int main(int argc, char** argv)
{
	std::cout << "----- Multi-Registration with Multiplier State -----" << std::endl;

	space_t space;

	Functions::AddParallel<state_t, MultiRegistrationFunction> f;
	f.createProblem(25);

	TestProblem(space, f);

	return 1;
}
