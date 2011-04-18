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
		: space_(space), noise_(noise), current_noise_(noise), is_partitioned_(false) {
		}

		void SetNoiseAmount(double alpha) {
			if(is_partitioned_) {
				// first suppress all
				for(size_t i=0; i<noise_.size(); ++i) {
					current_noise_[i] = alpha * suppression_ * noise_[i];
				}
				// now set correct noise for partition
				for(size_t i=0; i<partition_.size(); i++) {
					size_t index = partition_[i];
					current_noise_[index] = alpha * noise_[index];
				}
			} else {
				// no partition
				for(size_t i=0; i<noise_.size(); ++i) {
					current_noise_[i] = alpha * noise_[i];
				}
			}
			//std::cout << "Noise vector: " << current_noise_ << std::endl;
		}

		void DisablePartition() {
			is_partitioned_ = false;
		}

		void SetPartition(const std::vector<size_t>& partition, double suppression=0.01) {
			is_partitioned_ = true;
			partition_ = partition;
			suppression_ = suppression;
		}

		State operator()(const State& state) const {
			return space_.random(state, current_noise_);
		}

	private:
		const Space& space_;
		const std::vector<double>& noise_;
		std::vector<double> current_noise_;
		bool is_partitioned_;
		std::vector<size_t> partition_;
		double suppression_;

	};
}


//----------------------------------------------------------------------------//
}
//----------------------------------------------------------------------------//
#endif
