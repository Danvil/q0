/*
 * Solve.h
 *
 *  Created on: Sep 11, 2010
 *      Author: david
 */

#ifndef Q0_SOLVE_H_
#define Q0_SOLVE_H_

#include <QuestZero/Policies/TracePolicy.h>
#include <QuestZero/Optimization/Algorithms/RND.h>
#include <QuestZero/Optimization/Algorithms/PSO.h>
#include <QuestZero/Optimization/Algorithms/NelderMead.h>
#include <QuestZero/Optimization/Functions.h>
#include <QuestZero/Optimization/Optimization.h>
#include "../Timer.h"
#include <Eigen/Dense>
#include <iostream>
using namespace Q0;

template<typename State, typename Score>
struct AlgoTestResult
{
	std::string name;
	State state;
	Score score;
	double time_ms;
};

void printHeader(std::ostream& os) {
	std::cout << std::setw(32) << "Algorithm" << std::setw(16) << "Score" << std::setw(16) << "Time [ms]" << std::endl;
	std::cout << std::setw(32) << "--------------------------------" << std::setw(16) << "--------------" << std::setw(16) << "--------------" << std::endl;
}

template<typename State, typename Score>
std::ostream& operator<<(std::ostream& os, const AlgoTestResult<State,Score>& x) {
	std::cout << std::setw(32) << x.name << std::setw(16) << x.score << std::setw(16) << x.time_ms;
	return os;
}

//template<typename State, typename Score>
//void PrintAlgoTestResults(const std::vector<AlgoTestResult<State,Score>>& results) {
//	std::cout << std::setw(32) << "Algorithm" << std::setw(16) << "Score" << std::setw(16) << "Time [ms]" << std::endl;
//	for(const AlgoTestResult<State,Score>& x : results) {
//		std::cout << std::setw(32) << x.name << std::setw(16) << x.score << std::setw(16) << x.time_ms << std::endl;
//	}
//}

template<typename ALGO, class Space, class Function>
AlgoTestResult<typename ALGO::state_t, typename ALGO::score_t> TestAlgo(ALGO algo, const Space& space, const Function& function)
{
	Danvil::Timer timer;
	timer.start();
	TSample<typename ALGO::state_t, typename ALGO::score_t> best = algo.Minimize(space, function);
	timer.stop();
	return AlgoTestResult<typename ALGO::state_t, typename ALGO::score_t>{"", best.state, best.score, timer.getElapsedTimeInMilliSec()};
}

const unsigned int cIterationCount = 10;
//const double cScoreGoal = 1e-3;

template<typename State>
void printState(const State& x) {
	std::cout << x << std::endl;
}

template<typename K, int N, int _Options, int _MaxRows, int _MaxCols>
void printState(const Eigen::Matrix<K,N,1,_Options,_MaxRows,_MaxCols>& x) {
	std::cout << std::fixed << std::setprecision(5) << x.transpose() << std::endl;
}

template<class Space, class Function>
void TestProblem(const Space& space, const Function& function, unsigned int num_particles, bool print_result_state)
{
	printHeader(std::cout);

	typedef typename Function::Score score_t;
	typedef typename Space::State state_t;

	{
		Optimization<state_t, score_t,
				RND,
				Q0::InitializePolicy::RandomPicker<state_t>,
				Q0::ExitPolicy::FixedChecks<score_t>
		> algoRnd;
		algoRnd.SetIterationCount(cIterationCount);
		algoRnd.particleCount = num_particles / cIterationCount;
		auto tr = TestAlgo(algoRnd, space, function);
		tr.name = "RND";
		std::cout << tr << std::endl;
		if(print_result_state) {
			std::cout << "Result: ";
			printState(tr.state);
		}
	}

//	{
//		Optimization<state_t, score_t,
//				NelderMead,
//				Q0::InitializePolicy::ManyPicker<state_t,Q0::InitializePolicy::RandomPicker>,
//				Q0::ExitPolicy::FixedChecks<score_t>
//		> algoNM;
//		Q0::ExitPolicy::set_fixed_exit_policy(algoNM, num_particles / 4); // evaluates 4 particles per iteration
//		algoNM.p_simplex_size = 0.5;
//		algoNM.p_alpha = 1.0;
//		algoNM.p_beta = 0.4;
//		algoNM.p_gamma = 3.0;
//		auto tr = TestAlgo(algoNM, space, function);
//		tr.name = "Nelder Mead";
//		std::cout << tr << std::endl;
//		if(print_result_state) {
//			std::cout << "Result: ";
//			printState(tr.state);
//		}
//	}

	{
		Optimization<state_t, score_t,
				PSO,
				Q0::InitializePolicy::RandomPicker<state_t>,
				Q0::ExitPolicy::FixedChecks<score_t>
		> algoPso;
		algoPso.SetIterationCount(cIterationCount);
		algoPso.settings.particleCount = num_particles / cIterationCount;
		auto tr = TestAlgo(algoPso, space, function);
		tr.name = "PSO";
		std::cout << tr << std::endl;
		if(print_result_state) {
			std::cout << "Result: ";
			printState(tr.state);
		}
	}

}

template<typename Space, typename Function>
void Solve(const Space& space, const Function& function, unsigned int num_particles, bool verbose)
{
	typedef typename Space::State state_t;
	Q0::Functions::BoostFunctionSingleWrapper<state_t,double> f;
	f.set_functor([function](const state_t& x) { return function(x); });
	TestProblem(space, f, num_particles, verbose);
}

#endif
