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
#include <Danvil/Tools/Log.h>
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
			iterations_ = 100;
			goal_ = 0.5;
		}

		double step_size_;

		unsigned int iterations_;

		double goal_;
	};

	Settings settings_;

	std::string name() const { return "HillClimbing"; }

	template<class Space, class Function>
	Sample Optimize(const State& start, const Space& space, const Function& function, const std::vector<double>& noise) {
		State current = start;
		Score current_score = Score(0);
		size_t dimension = noise.size();
		SampleSet samples;
		std::vector<State> deltas;
		deltas.resize(dimension * 2 + 1);
		samples.Resize(dimension * 2 + 1);
		// iterate through layers
		for(unsigned int m=0; ; m++) {
			// evaluate current
			samples.set_state(0, current);
			// for each component create two states which deviate in this direction
			for(size_t i=0; i<dimension; i++) {
				double step = settings_.step_size_*noise[i];
				State d1 = space.unit(i, -step);
				State d2 = space.unit(i, +step);
				deltas[2*i + 1] = d1;
				deltas[2*i + 2] = d2;
				samples.set_state(2*i + 1, space.compose(current, d1));
				samples.set_state(2*i + 2, space.compose(current, d2));
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
			) {
				return Sample(current, current_score);
			}
			// move in the winning directions
			for(size_t i=0; i<dimension; i++) {
				Score s1 = samples.score(2*i + 1);
				Score s2 = samples.score(2*i + 2);
				if(IsBetter<Score,DoMinimize>(s1, s2)) {
					if(IsBetter<Score,DoMinimize>(s1, current_score)) {
						// it only gets better
						current = space.compose(current, deltas[2*i + 1]);
					}
					else {
						// we are near an optimum
						current = space.compose(current, deltas[2*i + 1]);
					}
				}
				else {
					current = space.compose(current, deltas[2*i + 2]);
				}
			}
		}
	}

};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
