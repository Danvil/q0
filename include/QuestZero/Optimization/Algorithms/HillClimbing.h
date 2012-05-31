/*
 * HillClimbing.h
 *
 *  Created on: April 25, 2010
 *      Author: david
 */

#ifndef Q0_ALGORITHMS_HILLCLIMBING_H_
#define Q0_ALGORITHMS_HILLCLIMBING_H_
//---------------------------------------------------------------------------
#include "QuestZero/Common/SampleSet.h"
#include "QuestZero/Common/ScoreComparer.h"
#include <QuestZero/Common/Log.h>
#include <vector>
#include <cassert>
#include <cmath>
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

/**
 *
 */
template<
	typename State,
	typename Score,
	class NotifySamples,
	int DoMinimize=false
>
struct HillClimbing
: public NotifySamples
{
	typedef TSample<State, Score> Sample;

	typedef TSampleSet<State, Score> SampleSet;

	struct Settings
	{
		Settings() {
			step_size_ = 1.0;
			step_size_min_ = 0.1;
			step_size_max_ = 2.0;
			step_size_reduce_ = 0.7;
			iterations_ = 100;
			goal_ = 0.5;
		}

		double step_size_;

		double step_size_reduce_;

		double step_size_min_;

		double step_size_max_;

		unsigned int iterations_;

		double goal_;
	};

	Settings settings_;

	std::string name() const { return "HillClimbing"; }

	struct StepSizePolicy
	{
		double operator()(size_t i) {
			return s_[i];
		}

	private:
		std::vector<double> s_;
	};

	template<class Space, class Function>
	Sample OptimizeParallel(const State& start, const Space& space, const Function& function, const std::vector<double>& noise) {
		double cAccuracy = 0.001;
		double cAcceleration = 1.2;
		State current = start;
		Score current_score = Score(0);
		Score last_score = Score(0);
		const size_t cCandidateCount = 4;
		double cCandidates[5] = {
				-cAcceleration,
				-1.0 / cAcceleration,
				1.0 / cAcceleration,
				cAcceleration
		};
		size_t dimension = space.dimension();
		if(noise.size() != dimension) {
			throw std::runtime_error("HillClimbing");
		}
		SampleSet samples;
		std::vector<State> deltas;
		std::vector<float> steps;
		samples.Resize(1 + cCandidateCount * dimension);
		deltas.resize(1 + cCandidateCount * dimension);
		steps.resize(dimension);
		// initialize step size
		for(size_t i=0; i<dimension; i++) {
			steps[i] = settings_.step_size_;
		}
		// iterate through layers
		for(unsigned int m=0; ; m++) {
			// parallel version which may result in a reduced score

			// evaluate current
			samples.set_state(0, current);
			// for each component create two states which deviate in this direction
			for(size_t i=0; i<dimension; i++) {
				for(size_t k=0; k<cCandidateCount; k++) {
					double step = steps[i] * cCandidates[k] * noise[i];
					State delta = space.unit(i, step);
					deltas[1 + cCandidateCount*i + k] = delta;
					samples.set_state(
							1 + cCandidateCount*i + k,
							space.compose(current, delta));
				}
			}
			// evaluate states
			samples.EvaluateAll(function);
			current_score = samples.score(0);
			// notify resent steps
			this->NotifySamples(samples);
			//samples.printInLines(std::cout);
			// check break condition
			if(m >= settings_.iterations_
				|| IsBetter<Score,DoMinimize>(current_score, settings_.goal_)
				|| std::abs(current_score - last_score) < cAccuracy
			) {
				return Sample(current, current_score);
			}
			last_score = current_score;
			// move in the winning directions
			for(size_t i=0; i<dimension; i++) {
				size_t best_candidate = static_cast<size_t>(-1);
				Score best_score = current_score;
				for(size_t k=0; k<cCandidateCount; k++) {
					Score s = samples.score(1 + cCandidateCount*i + k);
					if(IsBetter<Score,DoMinimize>(s, best_score)) {
						best_candidate = k;
						best_score = s;
					}
				}
				if(best_candidate != static_cast<size_t>(-1)) {
					current = space.compose(current, deltas[1 + cCandidateCount*i + best_candidate]);
					steps[i] *= cCandidates[best_candidate];
				}
				else {
					steps[i] /= cAcceleration;
				}
			}
		}
	}

	template<class Space, class Function>
	Sample Optimize(const State& start, const Space& space, const Function& function, const std::vector<double>& noise) {
		double cAccuracy = 0.001;
		double cAcceleration = 1.2;
		State current = start;
		Score current_score = Score(0);
		Score last_score = Score(0);
		const size_t cCandidateCount = 5;
		double cCandidates[5] = {
				-cAcceleration,
				-1.0 / cAcceleration,
				0,
				1.0 / cAcceleration,
				cAcceleration
		};
		size_t dimension = space.dimension();
		if(noise.size() != dimension) {
			throw std::runtime_error("HillClimbing");
		}
		SampleSet samples;
		std::vector<float> steps;
		samples.Resize(cCandidateCount);
		steps.resize(dimension);
		// initialize step size
		for(size_t i=0; i<dimension; i++) {
			steps[i] = settings_.step_size_;
		}
		// iterate through layers
		for(unsigned int m=0; ; m++) {
			// go over all dimensions
			for(size_t i=0; i<dimension; i++) {
				// create states for all candidates
				for(size_t k=0; k<cCandidateCount; k++) {
					double step = steps[i] * cCandidates[k] * noise[i];
					State delta = space.unit(i, step);
					samples.set_state(k, space.compose(current, delta));
				}
				// evaluate states
				samples.ComputeLikelihood(function);
				// notify resent steps
				this->NotifySamples(samples);
				// find best candidate
				size_t best_candidate = 0;
				Score best_score = samples.score(0);
				for(size_t k=1; k<cCandidateCount; k++) {
					Score s = samples.score(k);
					if(IsBetter<Score,DoMinimize>(s, best_score)) {
						best_candidate = k;
						best_score = s;
					}
				}
				current_score = best_score;
				if(best_candidate != static_cast<size_t>(-1)) {
					current = samples.state(best_candidate);
					steps[i] *= cCandidates[best_candidate];
				}
				/*else {
					steps[i] /= cAcceleration;
				}*/
			}
			// check break condition
			if(m >= settings_.iterations_
				|| IsBetter<Score,DoMinimize>(current_score, settings_.goal_)
				|| std::abs(current_score - last_score) < cAccuracy
			) {
				return Sample(current, current_score, 1.0);
			}
			last_score = current_score;
		}
	}

};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
