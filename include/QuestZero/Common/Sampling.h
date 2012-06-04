/*
 * Sampling.h
 *
 *  Created on: May 31, 2012
 *      Author: david
 */

#ifndef Q0_COMMON_SAMPLING_H_
#define Q0_COMMON_SAMPLING_H_
//---------------------------------------------------------------------------
#include <vector>
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

struct EmptySampleSetException {};

struct CanNotNormalizeZeroListException {};

struct InvalidDistributionException {};

namespace SamplingTools
{
	/** Normalizes a list of elements */
	template<typename K>
	double Sum(const std::vector<K>& v) {
		return std::accumulate(v.begin(), v.end(), 0.0, [](double a, K v) { return a + static_cast<double>(v); } );
	}

	/** Normalizes a list of elements */
	template<typename K>
	std::vector<double> Normalize(const std::vector<K>& v, double sum) {
		if(sum == 0) {
			throw CanNotNormalizeZeroListException();
		}
		double scl = 1.0 / sum;
		std::vector<double> weights;
		weights.resize(v.size());
		for(size_t i=0; i<v.size(); i++) {
			weights[i] = static_cast<double>(v[i]) * scl;
		}
		return weights;
	}

	/** Normalizes a list of elements */
	template<typename K>
	std::vector<double> Normalize(const std::vector<K>& v) {
		return Normalize(v, Sum(v));
	}

	/** Computes the non-unit density function of a discrete probability distribution
	 * Non-unit means that the distribution is not normalized to be 1
	 */
	template<typename K>
	static std::vector<double> ComputeDensityUnnormalized(const std::vector<K>& distribution) {
		// we first build the density using the unnormalized weights
		std::vector<double> density;
		density.reserve(distribution.size());
		double last = 0;
		for(size_t i=0; i<distribution.size(); i++) {
			last += double(distribution[i]);
			density.push_back(last);
		}
		// 'last' is now the total sum of all scores
		return density;
	}

	/** Computes the density function of a discrete probability distribution */
	template<typename K>
	std::vector<double> ComputeDensity(const std::vector<K>& scores) {
		// we first build the density using the unnormalized scores
		std::vector<double> density = ComputeDensityUnnormalized(scores);
		// normalize density
		return Normalize(density, density.back());
	}

	/** Randomly picks one elements from a density function of a discrete probability distribution
	 * The density vector does need to be normalized but it's values must be ordered!
	 */
	inline
	size_t SampleDensity(const std::vector<double>& density) {
		if(density.size() == 0 || density.back() == 0.0) {
			throw InvalidDistributionException();
		}
		// get random value in density
		double r = RandomNumbers::Uniform<double>(density.back());
		// find lower bound (first in density which has value >= r)
		auto it = std::lower_bound(density.begin(), density.end(), r);
		// return index to lower bound
		return it - density.begin();
	}

	/** Randomly picks several elements from a density function of a discrete probability distribution */
	inline
	std::vector<size_t> SampleDensity(const std::vector<double>& density, size_t count) {
		std::vector<size_t> picked(count);
		std::generate(picked.begin(), picked.end(), [&density]() { return SampleDensity(density); });
		return picked;
	}

	/** Randomly picks several elements from a discrete probability distribution */
	template<typename K>
	std::vector<size_t> DrawFromDistribution(const std::vector<K>& distribution, size_t count) {
		if(distribution.size() == 0) {
			throw InvalidDistributionException();
		}
		std::vector<double> density = ComputeDensityUnnormalized(distribution);
		if(density.back() == 0.0) {
			throw InvalidDistributionException();
		}
		return SampleDensity(density, count);
	}

}

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------

#endif
