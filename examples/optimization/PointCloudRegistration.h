/*
 * PointCloudRegistration.h
 *
 *  Created on: Sep 5, 2010
 *      Author: david
 */

#ifndef QUESTZERO_BENCHMARKS_POINTCLOUDREGISTRATION_H_
#define QUESTZERO_BENCHMARKS_POINTCLOUDREGISTRATION_H_

#include "QuestZero/Common/RandomNumbers.h"
#include <QuestZero/Common/SO3.h>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <vector>
#include <cmath>
#include <iostream>

namespace Benchmarks
{

template<typename K>
class PointCloudRegistration
{
public:
	typedef Eigen::Matrix<K,3,1> V;
	typedef Eigen::Matrix<K,3,3> M;

private:
	std::vector<V> original;

private:
	std::vector<V> target;

public:
	size_t pointCount() const { return original.size(); }

public:
	PointCloudRegistration()
	{}

	PointCloudRegistration(size_t point_count) {
		createRandomProblem(point_count);
	}

public:
	void createRandomProblem(size_t point_count) {
		const K cRange = 10.0;
		const K cNoise = 0.01;
		//M R = AxisAngle.Random(Optimization.RandomNumbers.Generator).ToMatrix(); // FIXME why not working in mono?
		Eigen::Quaternion<K> q = Q0::SO3::UniformRandom<K>(&RandomNumbers::Uniform<K>);
		LOG_INFO << "Registration rotation: " << q.coeffs();
		M R = q.toRotationMatrix();
		for(size_t i=0; i<point_count; i++) {
			V x(
				RandomNumbers::UniformMP(cRange),
				RandomNumbers::UniformMP(cRange),
				RandomNumbers::UniformMP(cRange));
			V noise(
				RandomNumbers::UniformMP(cNoise),
				RandomNumbers::UniformMP(cNoise),
				RandomNumbers::UniformMP(cNoise));
			original.push_back(x);
			target.push_back(R * x + noise);
		}
	}

public:
	double fit(const M& R) const {
		double sum = 0;
		for(size_t i=0; i<pointCount(); i++) {
			sum += (R * original[i] - target[i]).norm();
		}
		return std::sqrt(sum) / (double)pointCount();
	}

	double fit(const M& R, const V& t) const {
		double sum = 0;
		for(size_t i=0; i<pointCount(); i++) {
			sum += (R * original[i] + t - target[i]).norm();
		}
		return std::sqrt(sum) / (double)pointCount();
	}

};

}

#endif
