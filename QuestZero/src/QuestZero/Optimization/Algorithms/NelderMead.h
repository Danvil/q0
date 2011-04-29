/*
 * NelderMead.h
 *
 *  Created on: Apr 29, 2011
 *      Author: david
 */

#ifndef Q0_OPTIMIZATION_NELDERMEAD_H_
#define Q0_OPTIMIZATION_NELDERMEAD_H_
//----------------------------------------------------------------------------//
#include "QuestZero/Common/SampleSet.h"
#include "QuestZero/Common/ScoreComparer.h"
#include <vector>
#include <cassert>
#include <cmath>
//----------------------------------------------------------------------------//
namespace Q0 {
//----------------------------------------------------------------------------//

/// <summary>
/// Nelder Mead Downhill Simplex Optimization Algorithm
/// </summary>
template<typename State_, class Score_,
	class NotifySamples,
	bool Minimize
>
struct NelderMead
:  public NotifySamples
{
	typedef State_ State;
	typedef Score_ Score;
	typedef TSample<State,Score> Sample;
	typedef TSampleSet<State,Score> SampleSet;
	typedef typename ComparerSelector<Score,Minimize>::Result CMP;

	NelderMead() {
		p_simplex_size = 1.0;
		p_alpha = 1.0;
		p_beta = 0.5;
		p_gamma = 3.0;
		p_goal = 1e-5;
	}

	virtual ~NelderMead() {}

	std::string name() const { return "NelderMead"; }

	double p_simplex_size;
	double p_alpha;
	double p_beta;
	double p_gamma;
	double p_goal;

	template<class Space, class Function>
	Sample Optimize(const Space& space, const Function& function) {

		State initial = space.random();

		size_t n = space.dimension();

		double ha1 = (1.0 + p_alpha) / double(n - 1);
		double ha2 = - p_alpha;
		double hb1 = (1.0 + p_alpha) * (1.0 + p_gamma) / double(n - 1);
		double hb2 = p_gamma - p_alpha*(1.0 + p_gamma);
		double hc1 = (1.0 - p_beta) / double(n - 1);
		double hc2 = p_beta;
		double hd1 = (p_beta*(1.0 + p_alpha) + 1.0 - p_beta) / double(n - 1);
		double hd2 = - p_alpha * p_beta;

		SampleSet simplex;
		simplex.Resize(n + 1);
		std::vector<double> weights;
		weights.resize(n + 1);
		SampleSet extension;
		extension.Resize(4);

		// find initial simplex points
		simplex.set_state(0, initial);
		for(size_t i=0; i<n; i++) {
			double l = p_simplex_size;
			// FIXME find l s.t. Pi is in the domain?
			State Pi = space.compose(initial, space.unit(l));
			// FIXME is Pi in the domain?
			simplex.set_state(i+1, Pi);
		}

		while(true) {
			// evaluate all samples
			simplex.EvaluateAll(function);
			// determine best and worst simplex points
			size_t i_best, i_worst;
			simplex.template SearchBestAndWorst<CMP>(i_best, i_worst);
			Score y_l = simplex.score(i_best);
			Score y_h = simplex.score(i_worst);

			// form possible new samples
			// Form P_a = (1+a) P_mean - a P_worst
			// (top left)
			for(size_t i=0; i<n; i++) {
				weights[i] = (i != i_worst) ? ha1 : ha2;
			}
			State P_a = space.weightedSum(weights, simplex.states());
			extension.set_state(0, P_a);
			// Form P_b = (1+a)(1+c) P_mean - (a(1+c)-c) P_worst
			// (lower left)
			for(size_t i=0; i<n; i++) {
				weights[i] = (i != i_worst) ? hb1 : hb2;
			}
			State P_b = space.weightedSum(weights, simplex.states());
			extension.set_state(1, P_a);
			// Form P_c = (1-b) P_mean + b P_worst
			// (top right yes)
			for(size_t i=0; i<n; i++) {
				weights[i] = (i != i_worst) ? hc1 : hc2;
			}
			State P_c = space.weightedSum(weights, simplex.states());
			extension.set_state(2, P_a);
			// Form P_d = (b(1+a)+1-b) P_mean - a b P_worst
			// (top right no)
			for(size_t i=0; i<n; i++) {
				weights[i] = (i != i_worst) ? hd1 : hd2;
			}
			State P_d = space.weightedSum(weights, simplex.states());
			extension.set_state(3, P_a);
			// evaluate
			extension.EvaluateAll(function);
			Score y_a = extension.score(0);
			Score y_b = extension.score(1);
			Score y_c = extension.score(2);
			Score y_d = extension.score(3);
			if(CMP::compare(y_a, y_l)) {
				if(CMP::compare(y_b, y_l)) {
					simplex.set_state(i_worst, P_b);
					simplex.set_score(i_worst, y_b);
				}
				else {
					simplex.set_state(i_worst, P_a);
					simplex.set_score(i_worst, y_a);
				}
				i_best = i_worst; // we have stored a new best value there
			}
			else {
				// check if y_a is worse than all y_i except y_h
				bool new_worst = true;
				for(size_t i=0; i<n; i++) {
					if(i == i_worst) {
						continue;
					}
					if(CMP::compare(y_a, simplex.score(i))) {
						new_worst = false;
						break;
					}
				}
				if(new_worst) {
					State* Pu;
					Score yu;
					if(CMP::compare(y_h, y_a)) {
						// yes case
						Pu = &P_c;
						yu = y_c;
					}
					else {
						// no case
						Pu = &P_d;
						yu = y_d;
					}
					if(CMP::compare(y_h, yu)) {
						// reset the simplex
						for(size_t i=0; i<n; i++) {
							simplex.set_state(i, space.weightedSum(0.5, simplex.state(i), 0.5, simplex.state(i_best)));
						}
						simplex.EvaluateAll(function);
					}
					else {
						simplex.set_state(i_worst, *Pu);
						simplex.set_score(i_worst, yu);
					}
				}
				else {
					simplex.set_state(i_worst, P_a);
					simplex.set_score(i_worst, y_a);
				}
			}

			// check break condition
			double y_mean = 0;
			for(size_t i=0; i<n; i++) {
				y_mean += simplex.score(i);
			}
			y_mean /= double(n);
			double criterion = 0;
			for(size_t i=0; i<n; i++) {
				double x = simplex.score(i) - y_mean;
				criterion += x * x;
			}
			criterion /= double(n);
			if(std::sqrt(criterion) < p_goal) {
				//ready!!
				return simplex.CreateSample(i_best);
			}
		}
	}
};

//----------------------------------------------------------------------------//
}
//----------------------------------------------------------------------------//
#endif
