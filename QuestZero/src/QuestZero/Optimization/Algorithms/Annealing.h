/*
 * Annealing.h
 *
 *  Created on: Sep 15, 2010
 *      Author: david
 */

#ifndef ALGORITHMS_ANNEALING_H_
#define ALGORITHMS_ANNEALING_H_
//---------------------------------------------------------------------------
#include "QuestZero/Common/SampleSet.h"
#include "QuestZero/Optimization/Deterministic/BisectionSearch.h"
#include <Danvil/Tools/Log.h>
#include <vector>
#include <cassert>
#include <cmath>
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

/**
 * This is after "Articulated Body Motion Capture by Annealed Particle Filtering" by Deutscher, Blake and Reid
 * Mainly used in the annealed particle filter tracker.
 */
template<
	typename State,
	typename Score,
	class NotifySamples
>
struct ParticleAnnealing
: public NotifySamples
{
	typedef TSample<State, Score> Sample;

	typedef TSampleSet<State, Score> SampleSet;

	struct Settings
	{
		Settings() {
//			particleCount = 100;
			layers_ = 10;
			alpha_ = 0.5;
		}

//		/** Number of particles used */
//		unsigned int particle_count_;

		/** The number of annealing layers */
		unsigned int layers_;

		/** Particle survival rate which is a good measure for the rate of annealing */
		double alpha_;

		/** Noise vector */
		std::vector<double> noise_;
	};

	Settings settings_;

	std::string name() const { return "Annealing"; }

	template<class Space, class Function>
	SampleSet optimize(const SampleSet& start, const Space& space, const Function& function) {
		// prepare sample set
		SampleSet current = start;
		// FIXME what is the particle count?
//		assert(current.size() == settings_.particle_count_);
		// prepare noise
		std::vector<double> noise = settings_.noise_;
		// iterate through layers
		for(int m = settings_.layers_; m > 0; --m) {
			double alpha = settings_.alpha_;
			// apply noise scaling
			for(size_t i=0; i<noise.size(); ++i) {
				noise[i] *= alpha;
			}
			current.addNoise(space, noise);
			// find particle scores
			current.evaluateUnknown(function);
			// find best beta with respect to current scores
			double beta = BetaOptimizationProblem<Score>::Optimize_Bisect(alpha, current.scores(), 1e-2);
			LOG_DEBUG << "Annealing " << (settings_.layers_ - m + 1) << "/" << settings_.layers_ << ": Beta=" << beta;
			if(beta > BetaOptimizationProblem<Score>::cMaxBeta() * 0.99) {
				// we can not distinguish the scores anymore, so we assume that we have enough accuracy and quit
				// FIXME is this test good? this imposes some kind of scale on the score!
				break;
			}
			// map scores accordingly to found beta value
			current.MapScores(ExpScoreMapper(beta));
			// create new sample set using weighted random drawing
			current = current.DrawByScore(current.count());
			// notify about samples
			this->NotifySamples(current);
			// FIXME do we have to break early? what's wrong here?
			if(m == 0) {
				break;
			}
			// FIXME (old position of noise) is it good to move this to the front of the loop?
		}
		return current;
	}

	/** x |-> x^beta */
	struct ExpScoreMapper
	{
		ExpScoreMapper(double beta) : beta_(beta) {}
		double operator()(double x) const {
			return std::pow(x, beta_);
		}
	private:
		double beta_;
	};

private:
	template<typename T>
	struct BetaOptimizationProblem
	{
		struct AllScoresAreZeroException {};

		BetaOptimizationProblem(T alphaTarget, const std::vector<T>& scores)
		: alpha_target_(alphaTarget),
		  scores_(scores)
		{}

		static T cMaxBetaExp() { return 1; }
		static T cMaxBeta() { return 10; } // = 10^cMaxBetaExp

		/** Returns alpha_target - D(beta) / N (suitable for root finding) */
		T operator()(T u) const {
			T beta = StateToBeta(u);
			// x_i = s_i ^ beta
			// w = normalized(x)
			// D = 1 / sum{w_i^2}
			// first try:
//			T sum_1 = 0;
//			T sum_2 = 0;
//			for(typename std::vector<T>::const_iterator it=scores_.begin(); it!=scores_.end(); ++it) {
//				double x = std::pow(*it, beta);
//				sum_1 += x;
//				sum_2 += x * x;
//			}
//			T D = sum_1 * sum_1 / sum_2;
			// due to numerical instabilities, we must normalize first!
			std::vector<T> mapped = scores_;
			T mapped_sum = 0;
			T scores_sum = 0;
			for(typename std::vector<T>::iterator it=mapped.begin(); it!=mapped.end(); ++it) {
				const T& x = *it;
				scores_sum += x;
				T y = std::pow(x, beta);
				mapped_sum += y;
				*it = y;
			}
			if(scores_sum == 0) {
				throw AllScoresAreZeroException();
			}
			if(mapped_sum == 0) {
				// note that we excluded the case, that the sum of the original scores is 0
				// this comes from an extremely high beta. which should result in an alpha of 0
				return alpha_target_ - 0;
			}
			T mapped_sum_inv = 1 / mapped_sum;
			T D_sum = 0;
			for(typename std::vector<T>::const_iterator it=mapped.begin(); it!=mapped.end(); ++it) {
				T w = mapped_sum_inv * (*it);
				D_sum += w * w;
			}
			// compute current alpha
			T D = 1 / D_sum;
			T alpha = D / (T)scores_.size();
			// deviation from desired alpha
			return alpha_target_ - alpha;
		}

		typedef T StateT;

		typedef T ResultT;

		static T Optimize_Bisect(T alphaTarget, const std::vector<T>& scores, T epsilon) {
			try {
				BetaOptimizationProblem bop(alphaTarget, scores);
				double u_best = BisectionRootFinder::Solve(bop, -cMaxBetaExp(), +cMaxBetaExp(), epsilon);
				// FIXME why is this not working? linker error?
				return StateToBeta(u_best);
			} catch(BisectionRootFinder::NoSlopeException&) {
				return cMaxBeta();
			}
		}

	private:
		T alpha_target_;

		std::vector<T> scores_;

		static T StateToBeta(T x) {
			return pow(10.0, x);
		}

	};

};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
