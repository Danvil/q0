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
#include <vector>
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

/** Condensation algorithm after Isard and Black
 * This algorithms tries to maximize the function values.
 * The number of function evaluations per time step is exactly equal to the number of particles.
 */
template<typename ParticleSet>
struct CondensationIterative
{
private:
	ParticleSet particles_;

	unsigned int num_particles_;

	unsigned int time_;

public:
	CondensationIterative() {}

	/** Initializes the tracker by setting space and motion model */
	CondensationIterative(const ParticleSet& initial, unsigned int num_particles)
	: particles_(initial),
	  num_particles_(num_particles),
	  time_(0)
	{}

	/** Runs one iteration
	 * @throws InvalidDistributionException
	 * @return true if resample was successful, false otherwise
	 */
	template<typename MotionModel, typename Function>
	void iterate(const MotionModel& motion, const Function& f) {
		time_ ++;
		// apply motion model
		transform_states(particles_, motion);
		// evaluate samples
		compute_likelihood(particles_, f);
		// resample using weighted random drawing
		particles_ = draw_by_score(particles_, num_particles_);
	}

	/** Current tracking time */
	unsigned int get_time() const {
		return time_;
	}

	/** Returns all particles */
	const ParticleSet& get_particles() const {
		return particles_;
	}
};

template<typename ParticleSet>
CondensationIterative<ParticleSet> make_iterative_condensation(const ParticleSet& initial, unsigned int num_particles) {
	return CondensationIterative<ParticleSet>(initial, num_particles);
}

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
