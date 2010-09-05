/*
 * PointCloudRegistration.h
 *
 *  Created on: Sep 5, 2010
 *      Author: david
 */

#ifndef QUESTZERO_BENCHMARKS_POINTCLOUDREGISTRATION_H_
#define QUESTZERO_BENCHMARKS_POINTCLOUDREGISTRATION_H_

#include "QuestZero/RandomNumbers.h"
#include <Danvil/LinAlg.h>
#include <Danvil/LinAlg/RotationTools.h>
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
	PointCloudRegistration(size_t point_count) {
		createRandomProblem(point_count);
	}

public:
	void createRandomProblem(size_t point_count) {
		const K cRange = 10.0;
		const K cNoise = 0.01;
		//M R = AxisAngle.Random(Optimization.RandomNumbers.Generator).ToMatrix(); // FIXME why not working in mono?
		Danvil::ctLinAlg::TQuaternion<K> q = Danvil::ctLinAlg::RotationTools::UniformRandom<K>(&RandomNumbers::Random01);
		cout << "Registration rotation: " << q << endl;
		M R = q.rotation();
		for(size_t i=0; i<point_count; i++) {
			V x(
				RandomNumbers::S.randomMP(cRange),
				RandomNumbers::S.randomMP(cRange),
				RandomNumbers::S.randomMP(cRange));
			V noise(
				RandomNumbers::S.randomMP(cNoise),
				RandomNumbers::S.randomMP(cNoise),
				RandomNumbers::S.randomMP(cNoise));
			original.push_back(x);
			target.push_back(R * x + noise);
		}
	}

public:
	double fit(const M& R) {
		double sum = 0;
		for(size_t i=0; i<pointCount(); i++) {
			sum += Danvil::ctLinAlg::Distance(R * original[i], target[i]);
		}
		return sqrt(sum) / (double)pointCount();
	}

};

}

#endif
