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
#include "QuestZero/Common/ScoreComparer.h"
#include <QuestZero/Common/Log.h>
#include <vector>
#include <cassert>
#include <cmath>
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

/**
 * This is after "Articulated Body Motion Capture by Annealed Particle Filtering" by Deutscher, Blake and Reid
 * Mainly used in the annealed particle filter tracker.
 * Warning: When DoHistogramImprove is disabled, scores must be in the range [0,1]!!
 */
template<
	typename State,
	typename Score,
	class NotifySamples,
	int DoMinimize=false,
	int DoHistogramImprove=false
>
struct ParticleAnnealing
: public NotifySamples
{
	typedef TSample<State, Score> Sample;

	typedef TSampleSet<State, Score> SampleSet;

	struct Settings
	{
		Settings() {
			layers_ = 10;
			alpha_ = 0.5;
			particle_count_ = 1000;
		}

		/** The number of annealing layers */
		unsigned int layers_;

		/** Particle survival rate which is a good measure for the rate of annealing */
		double alpha_;

		unsigned int particle_count_;
	};

	Settings settings_;

	std::string name() const { return "Annealing"; }

	void PreprocessScores(SampleSet& current) {
		// map scores accordingly to found beta value
		Score best, worst;
		if(DoMinimize) {
			current.template FindBestAndWorstScore<BetterMeansSmaller<Score> >(best, worst);
		}
		else {
			current.template FindBestAndWorstScore<BetterMeansBigger<Score> >(best, worst);
		}
		current.TransformScores(RescaleMapper(best, worst));
	}

	template<class Space, class Function, class MotionModel>
	void OptimizeInplace(SampleSet& current, const Space& space, const Function& function, MotionModel motion) {
		const double cMaximalPossibleNoiseFactor = 1.0;
		double scale = cMaximalPossibleNoiseFactor * (1.0 - settings_.alpha_); // starting value follows from geometric sum
		// assert that scores are evaluated!
		current.ComputeLikelihood(function);
		// iterate through layers
		for(int m=settings_.layers_; m>0 ; m--) {
			// find best beta with respect to current scores
			double beta = BetaOptimizationProblem<Score>::Optimize_Bisect(settings_.alpha_, current.scores(), 1e-2);
			LOG_DEBUG << "Annealing " << (settings_.layers_ - m + 1) << "/" << settings_.layers_ << ": Beta=" << beta;
			if(beta >= 1.0) {
				// we can not distinguish the scores anymore, so we assume that we have enough accuracy and quit
//				// before we quit we have to restore raw scores
//				current.SetAllScores(scores_raw);
//				break;
				// FIXME is this test good? this imposes some kind of scale on the score!
			}
			// apply beta
			current.TransformScores(ExpScoreMapper(beta));
			// create new sample set using weighted random drawing
			current.Resample(settings_.particle_count_);
			// apply noise scaling
			motion.SetNoiseScale(scale);
			scale *= settings_.alpha_;
			// apply motion model to states
			current.TransformStates(motion);
			// find particle scores
			current.ComputeLikelihood(function);
			this->NotifySamples(current);
		}
	}

	template<class Space, class Function, class MotionModel>
	SampleSet Optimize(const SampleSet& start, const Space& space, const Function& function, MotionModel motion) {
		SampleSet current = start;
		OptimizeInplace(current, space, function);
		return current;
	}

private:
	static Score map(Score x, Score p) {
		return std::pow(x, p);
	}

	/** x |-> (x - worst) / (best - worst) */
	struct RescaleMapper
	{
		RescaleMapper(Score best, Score worst) : best_(best), worst_(worst) {
			scl_ = Score(1) / (best - worst);
		}
		Score operator()(Score x) const {
			return (x - worst_) * scl_;
		}
	private:
		Score best_, worst_;
		Score scl_;
	};

	/** x |-> x^beta or (1-x)^beta */
	struct ExpScoreMapper
	{
		ExpScoreMapper(double beta) : beta_(Score(beta)) {}
		Score operator()(Score x) const {
			if(DoMinimize) {
				return map(Score(1) - x, beta_);
			}
			else {
				return map(x, beta_);
			}
		}
	private:
		Score beta_;
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

//		static T cMaxBetaExp() { return 1; }
//		static T cMaxBeta() { return 10; } // = 10^cMaxBetaExp

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
//			T scores_sum = 0;
			for(typename std::vector<T>::iterator it=mapped.begin(); it!=mapped.end(); ++it) {
				T x = *it;
//				scores_sum += x;
				T y = map(x, beta);
				mapped_sum += y;
				*it = y;
			}
//			if(scores_sum == 0) {
//				throw AllScoresAreZeroException();
//			}
			if(mapped_sum == 0) {
				// note that we excluded the case, that the sum of the original scores is 0
				// this comes from an extremely high beta. which should result in an alpha of 0
				return alpha_target_ - 0;
			}
			T mapped_sum_inv = 1 / mapped_sum;
			T D_sum = 0;
			for(typename std::vector<T>::const_iterator it=mapped.begin(); it!=mapped.end(); ++it) {
				T x = *it;
				T w = mapped_sum_inv * x;
				D_sum += w * w;
			}
			// compute current alpha
			T D = 1 / D_sum;
			T alpha = D / (T)scores_.size();

//			std::cout << u << "\t" << beta << "\t" << alpha << std::endl;

			// deviation from desired alpha
			return alpha_target_ - alpha;
		}

		typedef T StateT;

		typedef T ResultT;

		static T Optimize_Bisect(T alphaTarget, const std::vector<T>& scores, T epsilon) {
			try {
				BetaOptimizationProblem bop(alphaTarget, scores);
				double u_best = BisectionRootFinder::Solve(bop, -2.0, +1.0, epsilon);
				// FIXME why is this not working? linker error?
				return StateToBeta(u_best);
			} catch(BisectionRootFinder::NoSlopeException&) {
				//return cMaxBeta();
				return StateToBeta(1.0);
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

template<
	class State,
	class Score,
	class StartingStates,
	class Take,
	class NotifySamples,
	bool DoMinimize
>
struct Annealing
: public ParticleAnnealing<State, Score, NotifySamples, DoMinimize>,
  public StartingStates,
  public Take
{
	template<class Space, class Function, class MotionModel>
	TSample<State, Score> Optimize(const Space& space, const Function& function, MotionModel motion) {
		typedef typename ComparerSelector<Score,DoMinimize>::Result CMP;
		TSampleSet<State, Score> open(this->pickMany(space, this->settings_.particle_count_));
		this->OptimizeInplace(open, space, function, motion);
		return this->template take<Space, CMP>(space, open);
	}

};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
