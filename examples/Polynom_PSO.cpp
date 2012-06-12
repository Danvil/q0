/*
 * Polynom.cpp
 *
 *  Created on: Jun 6, 2012
 *      Author: david
 */

#include <QuestZero/Spaces/Cartesian.h>
#include <QuestZero/Optimization/PSO.h>
#include <QuestZero/Optimization/Optimization.h>
#include <iostream>

// the domain of the function to optimize
typedef float state_t;

// the co-domain of the function to optimize
typedef float score_t;

// the function to optimize
score_t polynom(state_t x) {
	return x*x*x*x - 2*x*x*x - 1;
}

int main()
{
	std::cout << "Computing minimum of polynom f(x) = x^4 - 2 x^3 - 1" << std::endl;
	std::cout << "Using Particle Swarm Optimization (PSO)" << std::endl;

	// The whole infinite 1-dim Cartesian space is used to search for a minimum.
	// Further down we specify that the algorithm picks initial samples randomly.
	// Per default random space samples are picked using a normal distribution.
	Q0::Spaces::InfiniteCartesianSpace<state_t> space;
	space.SetRandomStateNormalDistribution(0, 1);

	// the algorithm class is constructed using several template parameters
	Q0::Optimization<state_t, score_t,
			// PSO algorithm
			Q0::PSO,
			// initial samples are picked randomly
			Q0::InitializePolicy::RandomPicker<state_t>,
			// algorithm runs for a fixed number of iterations and plots progress to console
			Q0::ExitPolicy::FixedChecks<score_t,true>
	> algo;

	// setup algorithm to do 5 iterations
	algo.SetIterationCount(5);
	// setup algorithm to use 100 particles
	algo.settings.particleCount = 100;

	// find minimum
	Q0::Sample<state_t, score_t> best = algo.Minimize(space, &polynom);

	// print results
	std::cout << "Optimization result:" << std::endl;
	std::cout << "x_min = " << best.state << std::endl;
	std::cout << "f(x_min) = " << best.score << std::endl;
	std::cout << "Expected result:" << std::endl;
	std::cout << "x_min = " << 3.0/2.0 << std::endl;
	std::cout << "f(x_min) = " << -43.0/16.0 << std::endl;

}

