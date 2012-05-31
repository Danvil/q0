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
	class Take,
	class NotifySamples,
	bool Minimize
>
struct NelderMead
: public Take,
  public NotifySamples
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
	}

	virtual ~NelderMead() {}

	std::string name() const { return "NelderMead"; }

	double p_simplex_size;
	double p_alpha;
	double p_beta;
	double p_gamma;

	/** Computes mean of all simplex points except 'i_worst' */
	template<class Space>
	State ComputeMean(const Space& space, const SampleSet& simplex, size_t i_worst) {
		std::vector<double> weights(num_samples(simplex));
		double w = 1.0 / double(weights.size() - 1);
		for(size_t i=0; i<weights.size(); i++) {
			weights[i] = (i == i_worst) ? 0.0 : w;
		}
		return space.weightedSum(weights, get_state_list(simplex));
	}

	/** Computes f*(u - pivot) + pivot */
	template<class Space>
	State Mirror(const Space& space, const State& pivot, const State& u, double f) {
		return space.compose(
				space.scale(
						space.difference(u, pivot),
						f
				),
				pivot
		);
	}

	template<class Space, class Function>
	Sample Optimize(const Space& space, const Function& function) {

		// FIXME find starting point
		State initial = space.random();

		// the algorithm maintains a simplex of n+1 points
		size_t n = space.dimension();
		SampleSet simplex;

		// FIXME find initial simplex points
		// current method:
		// first point is the random initial point
		// other n points are taken by going from initial point into unit direction
		set_state(simplex, add_sample(simplex), initial);
		for(size_t i=0; i<n; i++) {
			State p_i = space.compose(
					space.unit(i, p_simplex_size),
					initial);
			set_state(simplex, add_sample(simplex), p_i);
		}
		// evaluate score for simplex points
		compute_likelihood(simplex, function); // TODO correct function?
		this->NotifySamples(simplex);

		// the algorithm requires us to compute at most 4 points
		// from which it selects a new candidate with which the
		// current worst simplex point is replaced
		SampleSet extension;
		add_samples(extension, 4);

		while(true) {
			// determine best and worst simplex points
			auto i_best = find_best_by_score(simplex, CMP());
			auto i_worst = find_worst_by_score(simplex, CMP());
			Score y_l = get_score(simplex, i_best);
			Score y_h = get_score(simplex, i_worst);

			const State& p_h = get_state(simplex, i_worst);

			// for all candidates the mean point of all current
			// simplex points except the worst point is required

			State p_m = ComputeMean(space, simplex, i_worst);

			// first candidate: p_a = (1 + a)*p_m - a*p_h
			//                      = p_m + a*(p_m - p_h)
			State p_a = space.compose(
					space.scale(
							space.difference(p_m, p_h),
							p_alpha),
					p_m);
			set_state(extension, 0, p_a);

			// second candidate: p_b = c*p_a + (1 - c)*p_m
			//                       = p_m + c*(p_a - p_m)
			State p_b = space.compose(
					space.scale(
							space.difference(p_a, p_m),
							p_gamma),
					p_m);
			set_state(extension, 1, p_b);

			// third candidate: p_c = b*p_h + (1 - b)*p_m
			//                      = p_m + b*(p_h - p_m)
			State p_c = space.compose(
					space.scale(
							space.difference(p_h, p_m),
							p_beta),
					p_m);
			set_state(extension, 2, p_c);

			// fourth candidate: p_d = b*p_a + (1 - b)*p_m
			//                       = p_m + b*(p_a - p_m)
			State p_d = space.compose(
					space.scale(
							space.difference(p_a, p_m),
							p_beta),
					p_m);
			set_state(extension, 3, p_d);

			// compute score of all candidates
			compute_likelihood(extension, function); // TODO correct function?
			this->NotifySamples(extension);
			Score y_a = get_score(extension, 0);
			Score y_b = get_score(extension, 1);
			Score y_c = get_score(extension, 2);
			Score y_d = get_score(extension, 3);

			// even some candidates will perhaps not be used
			// in the following computation, we compute all
			// 4 at once to make use of parallel evaluation
			// TODO is this really faster ?

			if(CMP::compare(y_a, y_l)) {
				if(CMP::compare(y_b, y_l)) {
					set_state(simplex, i_worst, p_b);
					set_score(simplex, i_worst, y_b);
					y_l = y_b;
				}
				else {
					set_state(simplex, i_worst, p_a);
					set_score(simplex, i_worst, y_a);
					y_l = y_a;
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
					if(CMP::compare(y_a, get_score(simplex, i))) {
						new_worst = false;
						break;
					}
				}
				if(new_worst) {
					State* pu;
					Score yu;
					if(CMP::compare(y_h, y_a)) {
						// yes case
						pu = &p_c;
						yu = y_c;
					}
					else {
						// no case
						pu = &p_d;
						yu = y_d;
					}
					if(CMP::compare(y_h, yu)) {
						// reset the simplex
						for(size_t i=0; i<n+1; i++) {
							set_state(simplex, i, space.weightedSum(0.5, get_state(simplex, i), 0.5, get_state(simplex, i_best)));
						}
						compute_likelihood(simplex, function); // TODO correct function?
						this->NotifySamples(simplex);
					}
					else {
						set_state(simplex, i_worst, *pu);
						set_score(simplex, i_worst, yu);
					}
				}
				else {
					set_state(simplex, i_worst, p_a);
					set_score(simplex, i_worst, y_a);
				}
			}

			this->NotifySamples(simplex);

			// check break condition
			double y_mean = 0;
			for(size_t i=0; i<n; i++) {
				y_mean += get_score(simplex, i);
			}
			y_mean /= double(n);
			double criterion = 0;
			for(size_t i=0; i<n; i++) {
				double x = get_score(simplex, i) - y_mean;
				criterion += x * x;
			}
			criterion /= double(n);
			if(this->IsTargetReached(y_l, std::sqrt(criterion))) {
				return get_sample(simplex, i_best);
			}
		}
	}
};

//----------------------------------------------------------------------------//
}
//----------------------------------------------------------------------------//
#endif
