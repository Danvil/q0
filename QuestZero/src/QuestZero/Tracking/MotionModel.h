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
		: space_(space), noise_(noise), noise_scale_(1.0), current_noise_(noise), is_partitioned_(false), suppression_(0.0) {
			UpdateNoiseVector();
		}

		void SetNoiseScale(double scale) {
			noise_scale_ = scale;
			UpdateNoiseVector();
		}

		void SetSuppressionFactor(double suppression) {
			suppression_ = suppression;
			UpdateNoiseVector();
		}

		void DisablePartition() {
			is_partitioned_ = false;
			UpdateNoiseVector();
		}

		void SetPartition(const std::vector<size_t>& partition) {
			is_partitioned_ = true;
			partition_ = partition;
			UpdateNoiseVector();
		}

		State operator()(const State& state) const {
			return space_.random(state, current_noise_);
		}

	private:
		void UpdateNoiseVector() {
			if(is_partitioned_) {
				// first suppress all
				for(size_t i=0; i<noise_.size(); ++i) {
					current_noise_[i] = noise_scale_ * suppression_ * noise_[i];
				}
				// now set correct noise for partition
				for(size_t i=0; i<partition_.size(); i++) {
					size_t index = partition_[i];
					current_noise_[index] = noise_scale_ * noise_[index];
				}
			} else {
				// no partition
				for(size_t i=0; i<noise_.size(); ++i) {
					current_noise_[i] = noise_scale_ * noise_[i];
				}
			}
		}

	private:
		const Space& space_;
		const std::vector<double>& noise_;
		double noise_scale_;
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
