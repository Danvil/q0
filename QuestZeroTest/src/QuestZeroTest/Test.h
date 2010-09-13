/*
 * Test.h
 *
 *  Created on: Sep 11, 2010
 *      Author: david
 */

#ifndef TEST_H_
#define TEST_H_

#include <QuestZero/Policies/TracePolicy/ProgressAndBestToConsoleTracer.h>
#include <QuestZero/Optimization/Algorithms/RND.h>
#include <QuestZero/Optimization/Algorithms/PSO.h>
#include <QuestZero/Optimization/Optimization.h>
#include <Danvil/Tools/Timer.h>
#include <Danvil/Ptr.h>
#include <iostream>
using std::cout;
using std::endl;

template<typename ALGO, class Space, class Function>
void TestAlgo(ALGO algo, const Space& space, const Function& function)
{
	Danvil::Timer timer;
	timer.start();
	TSample<typename ALGO::State> best = algo.optimize(space, function);
	timer.stop();
	cout << "Time: " << timer.getElapsedTimeInMilliSec() << " ms" << endl;
	cout << "Result: " << best << endl;
}

#ifdef DEBUG
	template<typename State> struct MyTracer : ProgressAndBestToConsoleTracer<State> {};
	const unsigned int Iterations = 10;
#else
	template<typename State> struct MyTracer : NoTracer<State> {};
	const unsigned int Iterations = 100;
#endif

//#define FIXED_CHECKS
#ifdef FIXED_CHECKS
	typedef TargetPolicy::FixedChecks<double,unsigned long> TargetChecker;
#else
	typedef TargetPolicy::ScoreTargetWithMaxChecks<double,unsigned long> TargetChecker;
#endif

template<class Space, class Function>
void TestProblem(const Space& space, const Function& function)
{
	cout << "----- RND -----" << endl;
	Optimization<typename Space::State, RND, TargetChecker, InitialStatesPolicy::RandomPicker, TakePolicy::TakeBest, MyTracer> algoRnd;
	algoRnd.setMaxCount(Iterations);
	algoRnd.particleCount = 1000;
	TestAlgo(algoRnd, space, function);

	cout << "----- PSO -----" << endl;
	Optimization<typename Space::State, PSO, TargetChecker, InitialStatesPolicy::RandomPicker, TakePolicy::TakeBest, MyTracer> algoPso;
	algoPso.setMaxCount(Iterations);
	algoPso.settings.particleCount = 1000;
	TestAlgo(algoPso, space, function);

	cout << endl;
}

#endif
