/*
 * Polynom.cpp
 *
 *  Created on: Jun 6, 2012
 *      Author: david
 */

#include <QuestZero/Spaces/Cartesian.h>
#include <QuestZero/Optimization/Algorithms/RND.h>
#include <QuestZero/Optimization/Optimization.h>
#include <QuestZero/Common/DefaultSampleList.h>
#include <QuestZero/Policies.hpp>
#include <iostream>

typedef float state_t;
typedef float score_t;

score_t polynom(state_t x) {
	return x*x*x*x - 2*x*x*x - 1;
}

int main()
{
	std::cout << "Computing minimum of polynom f(x) = x^4 - 2 x^3 - 1" << std::endl;

	Q0::Spaces::InfiniteCartesianSpace<state_t> space;

	std::cout << "Using Monte Carlo Random Sampling (RND)" << std::endl;

	Q0::Optimization<state_t, score_t,
			Q0::RND,
			Q0::InitializePolicy::ManyPicker<state_t,Q0::InitializePolicy::RandomPicker>,
			Q0::ExitPolicy::FixedChecks<score_t>
	> algo;
	Q0::ExitPolicy::set_fixed_exit_policy(algo, 5);
	algo.particleCount = 100;

	Q0::TSample<state_t, score_t> best = algo.Minimize(space, &polynom, Q0::TracePolicy::None());

	std::cout << "Optimization result:" << std::endl;
	std::cout << "x_min = " << best.state << std::endl;
	std::cout << "f(x_min) = " << best.score << std::endl;

	std::cout << "Expected result:" << std::endl;
	std::cout << "x_min = " << 3.0/2.0 << std::endl;
	std::cout << "f(x_min) = " << -43.0/16.0 << std::endl;

}

