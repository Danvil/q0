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
		double sum = 0;
		for(std::vector<double>::const_iterator it=v.begin(); it!=v.end(); ++it) {
			sum += double(*it);
		}
		return sum;
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
			weights[i] = double(v[i]) * scl;
		}
		return weights;
	}

	/** Normalizes a list of elements */
	template<typename K>
	std::vector<double> Normalize(const std::vector<K>& v) {
		double sum = Sum(v);
		return Normalize(v, sum);
	}

	/** Computes the non-unit density function of a discrete probability distribution
	 * Non-unit means that the distribution is not normalized to be 1
	 */
	template<typename K>
	static std::vector<double> ComputeDensityUnnormalized(const std::vector<K>& distribution) {
		if(distribution.size() == 0) {
			throw CanNotNormalizeZeroListException();
		}
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

	/** Finds the index of the first value larger than the given value */
	inline
	size_t FindIndexOfFirstLargerThan(const std::vector<double>& list, double val) {
		// TODO use bisection search here if list is sorted!
		for(std::vector<double>::const_iterator it=list.begin(); it!=list.end(); ++it) {
			if(*it >= val) {
				return it - list.begin();
			}
		}
		throw InvalidDistributionException(); // TODO throw a different exception here!
	}

	/** Randomly picks one elements from a density function of a discrete probability distribution */
	inline
	size_t SampleDensity(const std::vector<double>& density) {
		double r = RandomNumbers::Uniform<double>(density.back());
		return FindIndexOfFirstLargerThan(density, r);
		// TODO catch possible exceptions and transfer to InvalidDistributionException
	}

	/** Randomly picks several elements from a density function of a discrete probability distribution */
	inline
	std::vector<size_t> SampleDensity(const std::vector<double>& density, size_t count) {
		std::vector<size_t> picked;
		picked.reserve(count);
		for(size_t i=0; i<count; ++i) {
			size_t p = SampleDensity(density);
			picked.push_back(p);
		}
		return picked;
	}

	/** Randomly picks several elements from a discrete probability distribution */
	template<typename K>
	std::vector<size_t> DrawFromDistribution(const std::vector<K>& distribution, size_t count) {
		std::vector<double> density = ComputeDensityUnnormalized(distribution);
		return SampleDensity(density, count);
	}

}

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------

#endif
