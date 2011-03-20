/*
 * MotionModel.h
 *
 *  Created on: Mar 18, 2011
 *      Author: david
 */

#ifndef Q0_MOTIONMODEL_H_
#define Q0_MOTIONMODEL_H_
//----------------------------------------------------------------------------//
#include <vector>
#include <boost/function.hpp>
//----------------------------------------------------------------------------//
namespace Q0 {
//----------------------------------------------------------------------------//

namespace MotionModels
{
	template<class Space>
	struct SpaceRandomMotionModel
	{
		typedef typename Space::State State;

		SpaceRandomMotionModel(const Space& space, const std::vector<double>& noise)
		: space_(space), noise_(noise), current_noise_(noise) {
		}

		void SetNoiseAmount(double alpha) {
			for(size_t i=0; i<noise_.size(); ++i) {
				current_noise_[i] = alpha * noise_[i];
			}
		}

		State operator()(const State& state) const {
			return space_.random(state, current_noise_);
		}

	private:
		const Space& space_;
		const std::vector<double>& noise_;
		std::vector<double> current_noise_;

	};
}


//----------------------------------------------------------------------------//
}
//----------------------------------------------------------------------------//
#endif
