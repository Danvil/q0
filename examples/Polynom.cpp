/*
 * Polynom.cpp
 *
 *  Created on: Jun 6, 2012
 *      Author: david
 */

#include <QuestZero/Spaces/Cartesian.h>
#include <QuestZero/Optimization/Algorithms/PSO.h>
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

	Q0::Spaces::Cartesian::InfiniteCartesianSpace<state_t> space;

	std::cout << "Using Particle Swarm Optimization (PSO)" << std::endl;

	Q0::Optimization<state_t, score_t,
			Q0::PSO,
			Q0::TargetPolicy::ScoreTargetWithMaxChecks<score_t,false>,
			Q0::InitialStatesPolicy::Fuser<Q0::InitialStatesPolicy::RandomPicker>::Result,
			Q0::TakePolicy::TakeBest,
			Q0::TracePolicy::Samples::None<state_t,score_t> > algoPso;
	algoPso.SetMaximumIterationCount(1000);
	algoPso.SetTargetScore(1e-5);
	algoPso.settings.particleCount = 100;

	Q0::TSample<state_t, score_t> best = algoPso.Minimize(space, &polynom);

	std::cout << "Optimization result:" << std::endl;
	std::cout << "x_min = " << best.state << std::endl;
	std::cout << "f(x_min) = " << best.score << std::endl;

	std::cout << "Expected result:" << std::endl;
	std::cout << "x_min = " << 3.0/2.0 << std::endl;
	std::cout << "f(x_min) = " << -43.0/16.0 << std::endl;

}

