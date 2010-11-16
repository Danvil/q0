/*
 * PointCloudRegistration.h
 *
 *  Created on: Sep 5, 2010
 *      Author: david
 */

#ifndef QUESTZERO_BENCHMARKS_POINTCLOUDREGISTRATION_H_
#define QUESTZERO_BENCHMARKS_POINTCLOUDREGISTRATION_H_

#include "QuestZero/Common/RandomNumbers.h"
#include <Danvil/LinAlg.h>
#include <Danvil/SO3.h>
#include <vector>
#include <cmath>
#include <iostream>
using std::cout;
using std::endl;

namespace Benchmarks
{

template<typename K>
class PointCloudRegistration
{
public:
	typedef Danvil::ctLinAlg::Vec3<K> V;
	typedef Danvil::ctLinAlg::Mat3<K> M;

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
		Danvil::SO3::Quaternion<K> q = Danvil::SO3::RotationTools::UniformRandom<K>(&RandomNumbers::Uniform<K>);
		cout << "Registration rotation: " << q << endl;
		M R = Danvil::SO3::ConvertToMatrix(q);
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
			sum += Danvil::ctLinAlg::Distance(R * original[i], target[i]);
		}
		return sqrt(sum) / (double)pointCount();
	}

	double fit(const M& R, const V& t) const {
		double sum = 0;
		for(size_t i=0; i<pointCount(); i++) {
			sum += Danvil::ctLinAlg::Distance(R * original[i] + t, target[i]);
		}
		return sqrt(sum) / (double)pointCount();
	}

};

}

#endif
