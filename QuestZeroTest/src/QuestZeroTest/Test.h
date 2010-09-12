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
#else
	template<typename State> struct MyTracer : NoTracer<State> {};
#endif

template<class Space, class Function>
void TestProblem(const Space& space, const Function& function)
{
	cout << "----- RND -----" << endl;
	Optimization<typename Space::State, RND, InitialStatesPolicy::RandomPicker, TakePolicy::TakeBest, MyTracer> algoRnd;
	algoRnd.particleCount = 1000;
	algoRnd.maxIterations = 20;
	TestAlgo(algoRnd, space, function);

	cout << "----- PSO -----" << endl;
	Optimization<typename Space::State, PSO, InitialStatesPolicy::RandomPicker, TakePolicy::TakeBest, MyTracer> algoPso;
	algoPso.settings.particleCount = 1000;
	algoPso.settings.iterations = 20;
	TestAlgo(algoPso, space, function);

	cout << endl;
}

#endif
