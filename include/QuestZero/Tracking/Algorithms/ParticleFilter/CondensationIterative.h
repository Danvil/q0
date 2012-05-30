/*
 * CondensationIterative.h
 *
 *  Created on: Oct 24, 2011
 *  Changed on: Oct 24, 2011
 *      Author: david
 */

#ifndef Q0_TRACKING_ALGORITHMS_CONDENSATION_H_
#define Q0_TRACKING_ALGORITHMS_CONDENSATION_H_
//---------------------------------------------------------------------------
#include <QuestZero/Common/SampleSet.h>
#include <QuestZero/Tracking/MotionModel.h>
#include <QuestZero/Common/ScoreComparer.h>
#include <boost/function.hpp>
#include <vector>
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

/** Condensation algorithm after Isard and Black
 * This algorithms tries to maximize the function values.
 * The number of function evaluations per time step is exactly equal to the number of particles.
 */
template<typename State, typename Space, typename Score>
struct CondensationIterative
{
public:
	typedef TSampleSet<State, Score> SampleSet;
	typedef TSample<State, Score> Sample;
	typedef boost::function<std::vector<Score>(const std::vector<State>&)> FncEval;

	/** Initializes the tracker by setting space and motion model */
	CondensationIterative(const Space& space, const std::vector<double>& noise)
	: space_(space), noise_(noise), motion_(space, noise)
	{}

	~CondensationIterative() {}

	/** (Re-)starts the tracker */
	void Start(const State& start, unsigned int particle_count) {
		setParticleCount(particle_count);
		// initialize sample set with random samples
		std::vector<State> states;
		states.reserve(particle_count_);
		states.push_back(start);
		for(size_t i=0; i<particle_count_ - 1; i++) {
			states.push_back(space_.random(start, noise_));
		}
		particles_ = SampleSet(states);
		time_ = 0;
	}

	/** Changes the number of particles */
	void setParticleCount(unsigned int n) {
		particle_count_ = n;
		BOOST_ASSERT(particle_count_ > 0);
	}

	/** Changes the evaluation function */
	void setEvaluationFunction(FncEval f) {
		fnc_evaluate_ = f;
	}

	/** Runs one iteration
	 * @return true if resample was successful, false otherwise
	 */
	bool Iterate() {
		time_ ++;
		BOOST_ASSERT(fnc_evaluate_);
		// prepare iteration
		particles_.TransformStates(motion_);
		// evaluate samples
		particles_.ComputeLikelihood(fnc_evaluate_);
		// resample using weighted random drawing
		try{
			particles_.Resample(particle_count_);
		} catch(CanNotNormalizeZeroListException&) {
			LOG_WARNING << "All samples have a score of zero. This means the tracker lost the object!";
			// tracker lost the object
			return false;
		}
		return true;
	}

	/** Current tracking time */
	unsigned int getTime() const {
		return time_;
	}

	/** Returns all particles */
	const SampleSet& getParticles() const {
		return particles_;
	}

	/** Returns the particle with the largest score */
	Sample getBestParticle() const {
		return particles_.template FindBestSample<typename Q0::BetterMeansBigger<Score>>();
	}

private:
	const Space& space_;

	const std::vector<double>& noise_;

	MotionModels::SpaceRandomMotionModel<Space> motion_;

	unsigned long particle_count_;

	SampleSet particles_;

	unsigned int time_;

	boost::function<std::vector<Score>(const std::vector<State>&)> fnc_evaluate_;

};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
