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
#include <QuestZero/Policies/TracePolicy.h>
#include <Danvil/LinAlg.h>
#include <boost/bind.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
using std::cout;
using std::endl;
using namespace Q0;

const unsigned int Problem9Dimension = 10;
const unsigned int Problem9Particles = 1000;

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

	typedef double score;

	typedef TTimeRange<time> range;

	range FactorRange() {
		range r;
		r.setBegin(0);
		r.setEnd(10);
		return r;
	}

	typedef Danvil::ctLinAlg::Vec<float, Problem9Dimension> state;

	typedef Spaces::Cartesian::FiniteCartesianSpace<state> space;

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
		boost::function<score(const state&)> rawfnct = boost::bind(&Benchmarks::Cartesian<state>::Sphere, _1);
		boost::function<score(const state&)> basefnct = boost::bind(&score_mapper, rawfnct, _1);
		basef.set_functor(basefnct);
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

	struct SaveAllToFile
	{
		SaveAllToFile() {}
		void NotifySamples(const TSampleSet<state,score>& samples) {
			std::stringstream fn_ss;
			fn_ss << "/tmp/" << base_ << i_ << ".txt";
			i_++;
			std::ofstream ofs(fn_ss.str().c_str());
			Danvil::Print::VectorToStream vts;
			vts.setPlainTabStyle();
			typedef TSample<state,score> sample;
			BOOST_FOREACH(const sample& x, samples.samples()) {
				ofs << x.score();
				if(!only_scores_) {
					ofs << "\t";
					vts.print(ofs, x.state());
				}
				ofs << endl;
			}
		}
		void setBaseAndReset(const std::string& base, bool only_scores=true) {
			base_ = base;
			i_ = 0;
			only_scores_ = only_scores;
		}
	private:
		std::string base_;
		unsigned int i_;
		bool only_scores_;
	};
#define TRACER SaveAllToFile

//#ifdef DEBUG
//	template<typename State> struct MyTracer : ProgressAndBestToConsoleTracer<State, BetterMeansBigger<State> > {};
//#else
//	template<typename State> struct MyTracer : NoTracer<State> {};
//#endif
//	#define TRACER MyTracer<state>


	range r = FactorRange();
	space s = FactorSpace();
	var_function f = FactorFunction(r, s);

	template<bool UseAnnealing>
	void runChoice()
	{
		typedef InitialStatesPolicy::ManyPicker<state, InitialStatesPolicy::RepeatOne> init;
		ParticleFilter<time, state, score, init, TakePolicy::TakeBest<state,score>, TRACER, TracePolicy::Solution::None<time,state,score>, UseAnnealing> solver;
		if(UseAnnealing) {
			cout << "~ Annealing is _ON__ ~" << endl;
			solver.setBaseAndReset("samples_annealing_");
		} else {
			cout << "~ Annealing is _OFF_ ~" << endl;
			solver.setBaseAndReset("samples_normal_");
		}
		solver.particle_count_ = 1000;
		// initial value and noise
		solver.setDefaultState(f.start_state);
		// step noise
		double step_noise = 2 * std::sqrt(0.1);
		for(unsigned int i=0; i<state::Dimension; i++) {
			solver.noise_.push_back(step_noise);
		}
		TSolution<time, state, score> sol = solver.Track(r, s, f);
		cout << sol << endl;
		cout << "Solution mean score: " << sol.ComputeMeanScore() << endl;
	}

	void run() {
		cout << "----- Tracking of Sphere moving from " << f.start_state << " to " << f.end_state << " over 10 frames in Vec3f space ---- " << endl;
		runChoice<false>();
		runChoice<true>();
	}

}
