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
#include <QuestZero/Optimization/Algorithms/NelderMead.h>
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
	typedef TargetPolicy::ScoreTargetWithMaxChecks<BetterMeansSmaller<double>,double,true> TargetChecker;
#else
	typedef TargetPolicy::ScoreTargetWithMaxChecks<BetterMeansSmaller<double>,double,false> TargetChecker;
#endif

const unsigned int cIterationCount = 10;
const double cScoreGoal = 1e-3;
const unsigned int cParticleCount = 1000;

template<class Space, class Function>
void TestProblem(const Space& space, const Function& function)
{
	cout << "----- RND -----" << endl;
	Optimization<typename Space::State, typename Function::Score,
			RND,
			TargetChecker,
			InitialStatesPolicy::Fuser<InitialStatesPolicy::RandomPicker>::Result,
			TakePolicy::TakeBest,
			TracePolicy::Samples::None<typename Space::State, typename Function::Score> > algoRnd;
	algoRnd.SetMaximumIterationCount(cIterationCount);
	algoRnd.SetTargetScore(cScoreGoal);
	algoRnd.particleCount = cParticleCount;
	TestAlgo(algoRnd, space, function);

	cout << "----- Nelder Mead -----" << endl;
	NelderMead<typename Space::State, typename Function::Score,
			TargetChecker,
			TracePolicy::Samples::None<typename Space::State, typename Function::Score>,
			//TracePolicy::Samples::AllToConsole<typename Space::State, typename Function::Score>,
			true> algoNM;
//	algoRnd.SetMaximumIterationCount(cIterationCount);
//	algoRnd.SetTargetScore(cScoreGoal);
//	algoRnd.particleCount = cParticleCount;
	algoNM.SetTargetScore(1e-08);
	algoNM.SetMaximumIterationCount(cIterationCount * cParticleCount / 4); // evaluates 4 particles per iteration
	algoNM.p_simplex_size = 0.5;
	algoNM.p_alpha = 1.0;
	algoNM.p_beta = 0.4;
	algoNM.p_gamma = 3.0;
	TestAlgo(algoNM, space, function);

	cout << "----- PSO -----" << endl;
	Optimization<typename Space::State, typename Function::Score,
			PSO,
			TargetChecker,
			InitialStatesPolicy::Fuser<InitialStatesPolicy::RandomPicker>::Result,
			TakePolicy::TakeBest,
			TracePolicy::Samples::None<typename Space::State, typename Function::Score> > algoPso;
	algoPso.SetMaximumIterationCount(cIterationCount);
	algoRnd.SetTargetScore(cScoreGoal);
	algoPso.settings.particleCount = cParticleCount;
	TestAlgo(algoPso, space, function);

	cout << endl;
}

#endif