/*
 * Problem09.cpp
 *
 *  Created on: Sep 12, 2010
 *      Author: david
 */

#include "Benchmarks/Cartesian.h"
#include "QuestZero/Optimization/Functions.h"
#include "QuestZero/Spaces/Cartesian.h"
#include "QuestZero/Tracking/Algorithms/ParticleFilter/ParticleFilter.h"
#include "QuestZero/Tracking/Tracking.h"
#include <QuestZero/Policies/TracePolicy/ProgressAndBestToConsoleTracer.h>
#include <Danvil/LinAlg.h>
#include <boost/bind.hpp>
#include <iostream>
using std::cout;
using std::endl;

template<typename Time, typename State, class Space, class Function>
struct MovingMinima
{
	double operator()(const Time& t, const State& state) const {
		double p = range.percent(t);
		State x = space.weightedSum(1-p, start_state, p, end_state);
		State moved_state = space.difference(state, x);
		return f(moved_state);
	}

	std::vector<double> operator()(const Time& t, const std::vector<State>& states) const {
		double p = range.percent(t);
		State x = space.weightedSum(1-p, start_state, p, end_state);
		std::vector<State> moved_states = states;
		for(size_t i=0; i<moved_states.size(); ++i) {
			moved_states[i] = space.difference(moved_states[i], x);
		}
		return f(moved_states);
	}

	TTimeRange<Time> range;
	Space space;
	Function f;
	State start_state;
	State end_state;
};

namespace Problem09
{
	typedef long time;

	typedef TTimeRange<time> range;

	range FactorRange() {
		range r;
		r.setBegin(0);
		r.setEnd(10);
		return r;
	}

	typedef Danvil::ctLinAlg::Vec<float, 10> state;

	typedef Spaces::Cartesian::CartesianSpace<state> space;

	space FactorSpace() {
		space U;
		state range;
		for(unsigned int i=0; i<state::Dimension; i++) {
			range[i] = (state::ScalarType)(2 + i);
		}
		U.setDomainRange(range);
		return U;
	}

	typedef Functions::BoostFunctionSingleWrapper<state> base_function;

	double score_mapper(const boost::function<double(const state&)>& bf, const state& s) {
		return exp(-bf(s));
	}

	typedef MovingMinima<time, state, space, base_function> var_function;

	var_function FactorFunction(const range& r, const space& s) {
		base_function basef;
		boost::function<double(const state&)> rawfnct = boost::bind(&Benchmarks::Cartesian<state>::Sphere, _1);
		boost::function<double(const state&)> basefnct = boost::bind(&score_mapper, rawfnct, _1);
		basef.setFunctor(basefnct);
		var_function varf;
		varf.f = basef;
		varf.range = r;
		varf.space = s;
		//varf.start_state = state(-1 + 2 * RandomNumbers::Random01(), -1 + 2 * RandomNumbers::Random01(), -1 + 2 * RandomNumbers::Random01());
		//varf.end_state = state(-1 + 2 * RandomNumbers::Random01(), -1 + 2 * RandomNumbers::Random01(), -1 + 2 * RandomNumbers::Random01());
		varf.start_state = state::FactorRepeat(0);
		varf.end_state = state::FactorRepeat(1);
		return varf;
	}

#ifdef DEBUG
	template<typename State> struct MyTracer : ProgressAndBestToConsoleTracer<State, BetterMeansBigger<State> > {};
#else
	template<typename State> struct MyTracer : NoTracer<State> {};
#endif

	range r = FactorRange();
	space s = FactorSpace();
	var_function f = FactorFunction(r, s);

	template<bool UseAnnealing>
	void runChoice()
	{
		if(UseAnnealing) {
			cout << "~ Annealing is _ON__ ~" << endl;
		} else {
			cout << "~ Annealing is _OFF_ ~" << endl;
		}
		typedef InitialStatesPolicy::ManyPicker<state, InitialStatesPolicy::RepeatOne> init;
		ParticleFilter<time, state, init, TakePolicy::TakeBest<state>, MyTracer<state>, UseAnnealing> solver;
		solver.particle_count_ = 500;
		// initial value and noise
		solver.setDefaultState(f.start_state);
		// step noise
		for(unsigned int i=0; i<state::Dimension; i++) {
			solver.noise_.push_back(0.5);
		}
		TSolution<time, state> sol = solver.track(r, s, f);
		cout << sol << endl;
		cout << "Solution mean score: " << sol.meanScore() << endl;
	}

	void run() {
		cout << "----- Tracking of Sphere moving from " << f.start_state << " to " << f.end_state << " over 10 frames in Vec3f space ---- " << endl;
		runChoice<false>();
		runChoice<true>();
	}

}
