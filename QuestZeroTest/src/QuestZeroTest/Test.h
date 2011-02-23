/*
 * Test.h
 *
 *  Created on: Sep 11, 2010
 *      Author: david
 */

#ifndef TEST_H_
#define TEST_H_

#include <QuestZero/Policies/TracePolicy.h>
#include <QuestZero/Optimization/Algorithms/RND.h>
#include <QuestZero/Optimization/Algorithms/PSO.h>
#include <QuestZero/Optimization/Optimization.h>
#include <Danvil/Tools/Timer.h>
#include <Danvil/Ptr.h>
#include <iostream>
using std::cout;
using std::endl;
using namespace Q0;

template<typename ALGO, class Space, class Function>
void TestAlgo(ALGO algo, const Space& space, const Function& function)
{
	Danvil::Timer timer;
	timer.start();
	TSample<typename ALGO::State, typename ALGO::Score> best = algo.Optimize(space, function);
	timer.stop();
	cout << "Time: " << timer.getElapsedTimeInMilliSec() << " ms" << endl;
	cout << "Result: " << best << endl;
}

#ifdef DEBUG
	typedef TargetPolicy::ScoreTargetWithMaxChecks<double,true> TargetChecker;
#else
	typedef TargetPolicy::ScoreTargetWithMaxChecks<double,false> TargetChecker;
#endif

const unsigned int cIterationCount = 10;
const double cScoreGoal = 1e-3;
const unsigned int cParticleCount = 1000;

template<class Space, class Function>
void TestProblem(const Space& space, const Function& function)
{
	cout << "----- RND -----" << endl;
	Optimization<
		typename Space::State,
		typename Function::Score,
		RND,
		TargetChecker,
		InitialStatesPolicy::RandomPicker,
		TakePolicy::TakeBest,
		TracePolicy::Samples::None
	> algoRnd;
	algoRnd.SetMaximumIterationCount(cIterationCount);
	algoRnd.SetTargetScore(cScoreGoal);
	algoRnd.particleCount = cParticleCount;
	TestAlgo(algoRnd, space, function);

	cout << "----- PSO -----" << endl;
	Optimization<typename Space::State, typename Function::Score, PSO, TargetChecker, InitialStatesPolicy::RandomPicker, TakePolicy::TakeBest, TracePolicy::Samples::None> algoPso;
	algoPso.SetMaximumIterationCount(cIterationCount);
	algoRnd.SetTargetScore(cScoreGoal);
	algoPso.settings.particleCount = cParticleCount;
	TestAlgo(algoPso, space, function);

	cout << endl;
}

#endif
