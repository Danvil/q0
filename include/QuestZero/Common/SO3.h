/*
 * SO3.h
 *
 *  Created on: Sep 5, 2010
 *  Changed on: Jun 1, 2012
 *      Author: david
 */

#ifndef Q0_COMMON_SO3_H_
#define Q0_COMMON_SO3_H_
//---------------------------------------------------------------------------
#include "Tools.hpp"
#include "Log.h"
#include <Eigen/Geometry>
#include <boost/math/constants/constants.hpp>
#include <cmath>
//---------------------------------------------------------------------------
namespace Q0 {
namespace SO3 {
//---------------------------------------------------------------------------

/** Asserts that an angle lies in [-pi|+pi[ */
template<typename K>
K SmallAngleForm(K a) {
	return Wrap(a, -boost::math::constants::pi<K>(), +boost::math::constants::pi<K>());
}

template<typename K>
Eigen::Matrix<K,3,1> SmallAngleRodriguez(const Eigen::AngleAxis<K>& aa) {
	K a = Wrap(aa.angle(), 2 * boost::math::constants::pi<K>());
	if(a > boost::math::constants::pi<K>()) {
		a -= 2 * boost::math::constants::pi<K>();
	}
	return a * aa.axis();
}

template<typename K>
Eigen::Matrix<K,3,1> SmallAngleRodriguez(const Eigen::Matrix<K,3,1>& r) {
	K a = Wrap(r.norm(), 2 * boost::math::constants::pi<K>());
	if(a > boost::math::constants::pi<K>()) {
		a -= 2 * boost::math::constants::pi<K>();
	}
	return a * r.normalized();
}

/** The amount of rotation of a quaternion */
template<typename K>
K ShortestAngle(const Eigen::Quaternion<K>& q) {
	K u = K(2) * std::acos(std::min(K(1), q.w())); // protect acos against numerical instabilities in w
//	assert(u >= 0 && u <= K(2) * boost::math::constants::pi<K>());
	return std::min(u, K(2) * boost::math::constants::pi<K>() - u);
}

/** Returns an equal quaternion where w is positive */
template<typename K>
Eigen::Quaternion<K> SmallAngleForm(const Eigen::Quaternion<K>& q) {
	return q.w() < 0 ? -q : q;
}

/** Same as ExpR but returns a quaternion instead of a matrix */
template<typename K>
Eigen::Quaternion<K> ExpQ(const Eigen::Matrix<K,3,1>& v) {
	return Eigen::Quaternion<K>(Eigen::AngleAxis<K>(v.norm(), v.normalized()));
}

//	static mat3_t Hat(const vec3_t& v) {
//		return mat3_t(
//			0, -v.z(), v.y(),
//			v.z(), 0, -v.x(),
//			-v.y(), v.x(), 0);
//	}

/** Converts so(3) to SO(3)
 * See http://en.wikipedia.org/wiki/Axis_angle#Log_map_from_SO.283.29_to_so.283.29
 */
template<typename K>
Eigen::Matrix<K,3,3> ExpR(const Eigen::Matrix<K,3,1>& r) {
//		K phi = r.length();
//		if(phi == 0) {
//			return mat3_t::Identity();
//		}
//		vec3_t n = (1 / phi) * r;
//		mat3_t hn = Hat(n);
//		K sin_phi = std::sin(phi);
//		K cos_phi = std::cos(phi);
//		return mat3_t::Identity() + sin_phi * hn + (1 - cos_phi) * hn * hn;
	return ExpQ(r).toRotationMatrix();
}


/** Converts SO(3) to so(3) */
template<typename K>
Eigen::Matrix<K,3,1> Log(const Eigen::Quaternion<K>& q) {
	// convert quaternion to axis/angle form
	Eigen::AngleAxis<K> aa = q;
	// and get rodriguez form
	// TODO small angle form?
	return aa.angle() * aa.axis();
}

/** Converts SO(3) to so(3)
 * See http://en.wikipedia.org/wiki/Axis_angle#Log_map_from_SO.283.29_to_so.283.29
 */
template<typename K>
Eigen::Matrix<K,3,1> Log(const Eigen::Matrix<K,3,3>& R) {
//		K phi = acos(0.5 * (Trace(R) - 1));
//		if(phi == 0) {
//			return Vec3<K>::Zero();
//		}
//		else {
//			K scl = 1 / (2 * sin(phi));
//			Vec3<K> n(R[2, 1] - R[1, 2], R[0, 2] - R[2, 0], R[1, 0] - R[0, 1]);
//			return phi * scl * n;
//		}
	return Log(Eigen::Quaternion<K>(R));
}

/** Random uniformly distributed quaternion
 * @param gen source of uniformly distributed random numbers in [0,1]
 */
template<typename K>
Eigen::Quaternion<K> UniformRandom(K (*gen)()) {
	// trivial method: sample 4D points in the box [-1,+1] and take only those with radius <= 1
	// Probability per iteration is ~30.8%
	while(true) {
		K r1 = K(2) * gen() - K(1);
		K r2 = K(2) * gen() - K(1);
		K r3 = K(2) * gen() - K(1);
		K r4 = K(2) * gen() - K(1);
		K d = r1 * r1 + r2 * r2 + r3 * r3 + r4 * r4;
		if(d <= K(1) && d > K(0)) {
			K s = (K)1 / std::sqrt(d);
			return Eigen::Quaternion<K>(s * r1, s * r2, s * r3, s * r4);
		}
	}
//		// computational intensive method
//		K u1 = gen(), u2 = gen(), u3 = gen();
//		K su2, cu2, su3, cu3;
//		Danvil::MoreMath::SinCos(Danvil::TwoPi<K>() * u2, su2, cu2);
//		Danvil::MoreMath::SinCos(Danvil::TwoPi<K>() * u3, su3, cu3);
//		K p = std::sqrt(K(1) - u1);
//		K q = std::sqrt(u1);
//		return Quaternion<K>(p * su2, p * cu2, q * su3, q * cu3);
}

/** Random uniformly distributed quaternion with maximal angle 'len'
 * @param len maximal rotation angle
 * @param gen source of uniformly distributed random numbers in [0,1]
 */
template<typename K>
Eigen::Quaternion<K> UniformRandom(K len, K (*gen)()) {
	// trivial method: get a quaternion from the whole space, than re-scale the angle
	Eigen::Quaternion<K> u = UniformRandom(gen);
	if(len < boost::math::constants::pi<K>()) {
		// only rescale if we want less than the whole space
		Eigen::Matrix<K,3,1> v = Log(u);
		v *= len / boost::math::constants::pi<K>();
		return ExpQ(v);
	} else {
		return u;
	}
}

namespace Private
{
	template<typename K>
	struct EqualWeight
	{
		EqualWeight(size_t n)
		: scl_(K(1) / K(n)) {
		}
		K w(size_t) const {
			return scl_;
		}
		std::vector<K> weights() const {
			return std::vector<K>(1, scl_);
		}
	private:
		K scl_;
	};

	template<typename K>
	struct Weighted
	{
		template<typename W>
		Weighted(const std::vector<W>& weightsW)
		: weights_(weightsW.size()) {
			// normalize the weights
			W weights_sum = 0;
			for(size_t i=0; i<weightsW.size(); i++) {
				weights_sum += weightsW[i];
			}
			if(weights_sum == 0) {
				throw std::runtime_error("Sum of weights is zero!");
			}
			K scl = K(1) / K(weights_sum);
			for(size_t i=0; i<weights_.size(); i++) {
				weights_[i] = K(weightsW[i]) * scl;
			}
		}
		K w(size_t i) const {
			return weights_[i];
		}
		std::vector<K> weights() const {
			return weights_;
		}
	private:
		std::vector<K> weights_;
	};

//#define EXTRA_VERBOSE

	template<typename K, class WeightPolicy>
	static Eigen::Quaternion<K> MeanImpl3D(const std::vector<Eigen::Quaternion<K> >& qs, const WeightPolicy& wp, K accuracy) {
		typedef Eigen::Quaternion<K> quaternion_t;
		typedef Eigen::AngleAxis<K> angle_axis_t;
		typedef Eigen::Matrix<K,3,1> vec3_t;
		size_t cnt = qs.size();
//			// use small angle representation to always get the "shorter" rotation
//			std::vector<Quaternion<K> > qs_smallangle(cnt);
//			for(size_t i=0; i<cnt; i++) {
//				qs_smallangle[i] = qs[i].SmallAngleForm();
////				std::cout << "Vals: " << qs[i] << " - " << qs_smallangle[i] << std::endl;
//			}
		const int cMaxIterations = 1000;
		// run iterative algorithm
		quaternion_t y = qs[0]; // TODO is this a good starting value?
		quaternion_t y_last = y;
		//Quaternion<K> y = Quaternion<K>::Identity();
		int n = 0;
		while(true) {
#ifdef EXTRA_VERBOSE
			std::cout << "Iteration " << n << ":" << std::endl;
#endif
			Eigen::Quaternion<K> y_inv = y.inverse();
#ifdef EXTRA_VERBOSE
			std::cout << "y=" << y << std::endl;
			std::cout << "y^-1=" << y_inv << std::endl;
#endif
			vec3_t e = vec3_t::Zero();
			for(size_t i=0; i<cnt; i++) {
				// TODO the multiplication with the weight may be done outside the loop for some policies
				quaternion_t a = y_inv * qs[i]; //* qs_smallangle[i];
				vec3_t ei = Log(a);
				e += wp.w(i) * ei;
#ifdef EXTRA_VERBOSE
				std::cout << wp.w(i) << " - " << ei << " - " << a << std::endl;
#endif
			}
#ifdef EXTRA_VERBOSE
			std::cout << "e=" << e << std::endl;
#endif
			e = SmallAngleRodriguez(e);
#ifdef EXTRA_VERBOSE
			std::cout << "e=" << e << std::endl;
#endif
			// check if the value is not changing anymore
			if(e.length() < accuracy) {
				// no decide if we want this or the opposite
				// beside y the "opposite rotation" is also a solution
				// one is instable (and overall further away)
				// we want the right one!
				angle_axis_t y_aa(y);
				angle_axis_t y_prime_aa(y_aa.axis(), y_aa.angle() - boost::math::constants::pi<K>());
				quaternion_t y_prime(y_prime_aa);
				// sum up dinstances
				K error_1 = K(0);
				K error_2 = K(0);
				for(size_t i=0; i<cnt; i++) {
					error_1 += wp.w(i) * qs[i].angularDistance(y);
					error_2 += wp.w(i) * qs[i].angularDistance(y_prime);
				}
				// the one with shorter distance wins
				if(error_1 < error_2) {
					return SmallAngleForm(y).normalized();
				}
				else {
					return SmallAngleForm(y_prime).normalized();
				}
			}
			// next guess
			y = y * ExpQ(e);
			// we do not want to run this forever!
			n++;
			if(n == cMaxIterations) {
				LOG_ERROR << "Mean rotation algorithm did not converge!";
				LOG_ERROR << "Current: " << y;
				LOG_ERROR << "Last   : " << y_last;
				LOG_ERROR << "Quaternions: " << qs;
				LOG_ERROR << "Weights: " << wp.weights();
				throw std::runtime_error("Mean rotation algorithm did not converge!");
			}
			y_last = y;
		}
	}

	/** Same as MeanImpl but for the 2D case (where a rotation can be parameterized with one number) */
	template<typename K, class WeightPolicy>
	static K MeanImpl2D(const std::vector<K>& qs, const WeightPolicy& wp, K accuracy) {
		// use small angle representation to always get the "shorter" rotation
		size_t cnt = qs.size();
		std::vector<K> qs_smallangle(cnt);
		for(size_t i=0; i<cnt; i++) {
			qs_smallangle[i] = SmallAngleForm(qs[i]);
		}
		const int cMaxIterations = 1000;
		// run iterative algorithm
		K y = 0; // TODO is this a good starting value?
		//Quaternion<K> y = Quaternion<K>::Identity();
		int n = 0;
		while(true) {
			K e = 0;
			for(size_t i=0; i<cnt; i++) {
				// TODO the multiplication with the weight may be done outside the loop for some policies
				e += wp.w(i) * SmallAngleForm(qs_smallangle[i] - y); // like with quaternions,  inverse = operator- for angular
			}
			// check if the value is not changing anymore
			if(std::abs(e) < accuracy) {
				return y;
			}
			y += e;
			n++;
			if(n == cMaxIterations) {
				LOG_ERROR << "Mean rotation algorithm did not converge!";
				LOG_ERROR << "Weights -> Quaternions: ";
				for(unsigned int i=0; i<qs.size(); i++) {
					LOG_ERROR << wp.weights()[i] << " -> " << qs[i];
				}
				throw std::runtime_error("Mean rotation algorithm did not converge!");
			}
		}
	}

}

/** Computes weighted mean of 3D rotations (given as quaternions)
 * See http://en.wikipedia.org/wiki/Generalized_quaternion_interpolation
 */
template<typename K, typename W>
Eigen::Quaternion<K> WeightedMean(const std::vector<Eigen::Quaternion<K> >& qs, const std::vector<W>& weights, K accuracy=detail::Epsilon<K>()) {
	if(qs.size() == 0) {
		throw std::runtime_error("Can not compute mean of empty set of quaternions!");
	}
	if(weights.size() != qs.size()) {
		throw std::runtime_error("Weight count must match rotation count!");
	}
	return Private::MeanImpl3D(qs, Private::Weighted<K>(weights), accuracy);
}

/** Computes mean of 3D rotations (given as quaternions)
 * Same as WeightedMean with all weights equal to 1
 */
template<typename K>
Eigen::Quaternion<K> Mean(const std::vector<Eigen::Quaternion<K> >& qs, K accuracy=detail::Epsilon<K>()) {
	if(qs.size() == 0) {
		throw std::runtime_error("Can not compute mean of empty set of quaternions!");
	}
	return Private::MeanImpl3D(qs, Private::EqualWeight<K>(qs.size()), accuracy);
}

/** Computes weighted mean of 2D rotations (given as angles) */
template<typename K, typename W>
K WeightedMean(const std::vector<K>& qs, const std::vector<W>& weights, K accuracy=detail::Epsilon<K>()) {
	if(qs.size() == 0) {
		throw std::runtime_error("Can not compute mean of empty set of angles!");
	}
	if(weights.size() != qs.size()) {
		throw std::runtime_error("Weight count must match rotation count!");
	}
	return Private::MeanImpl2D(qs, Private::Weighted<K>(weights), accuracy);
}

/** Computes mean of 2D rotations (given as angles)
 * Same as WeightedMean with all weights equal to 1
 */
template<typename K>
K Mean(const std::vector<K>& qs, K accuracy=detail::Epsilon<K>()) {
	if(qs.size() == 0) {
		throw std::runtime_error("Can not compute mean of empty set of angles!");
	}
	return Private::MeanImpl2D(qs, Private::EqualWeight<K>(qs.size()), accuracy);
}

/** Computes mean of 2D rotations (given as angles)
 * Same as WeightedMean but for exactly three elements
 */
template<typename K>
K WeightedMean(K a1, K a2, K a3, K w1, K w2, K w3, K accuracy=detail::Epsilon<K>()) {

	// FIXME test function before using it!
	throw 0;

	// use small angle representation to always get the "shorter" rotation
	a1 = SmallAngleForm(a1);
	a2 = SmallAngleForm(a2);
	a3 = SmallAngleForm(a3);
	const int cMaxIterations = 1000;
	// run iterative algorithm
	K y = 0; // TODO is this a good starting value?
	unsigned int n = 0;
	while(true) {
		K e = w1 * SmallAngleForm(a1 - y) + w2 * SmallAngleForm(a2 - y) + w3 * SmallAngleForm(a3 - y);
		// check if the value is not changing anymore
		if(std::abs(e) < accuracy) {
			return y;
		}
		y += e;
		n++;
		if(n == cMaxIterations) {
			LOG_ERROR << "Mean rotation algorithm did not converge!";
			LOG_ERROR << "Angles: {" << a1 << ", " << a2 << ", a3" << "}";
			throw std::runtime_error("Mean rotation algorithm did not converge!");
		}
	}
}

//---------------------------------------------------------------------------
}}
//---------------------------------------------------------------------------
#endif
