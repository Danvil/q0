/*
 * SO3.h
 *
 *  Created on: Sep 10, 2010
 *      Author: david
 */

#ifndef SPACE_SO3_H_
#define SPACE_SO3_H_
//---------------------------------------------------------------------------
#include "BaseSpace.h"
#include "QuestZero/Common/RandomNumbers.h"
#include "QuestZero/Common/Exceptions.h"
#include <Danvil/LinAlg.h>
#include <Danvil/SO3.h>
#include <Danvil/Tools/MoreMath.h>
#include <Danvil/Tools/Field.h>
#include <Danvil/Ptr.h>
#include <boost/foreach.hpp>
#include <boost/range.hpp>
#include <vector>
#include <cassert>
#include <cmath>
//---------------------------------------------------------------------------
namespace Q0 {
namespace Spaces {
//---------------------------------------------------------------------------

namespace SO3 {

	namespace Operations
	{
		template<typename K>
		struct SO3Ops
		{
			typedef Danvil::SO3::Quaternion<K> State;

			double distance(const State& a, const State& b) const {
				return (double)State::Distance(a, b);
			}

			State scale(const State& a, double s) const {
				return Danvil::SO3::RotationTools::ExpQ(
						s * Danvil::SO3::RotationTools::Log(a));
			}

			State inverse(const State& a) const {
				return a.inverse();
			}

			State compose(const State& a, const State& b) const {
				return a * b;
			}

			// TODO this is the default implementation
			State difference(const State& a, const State& b) const {
				return compose(a, inverse(b));
			}

			// TODO this is the default implementation
			template<typename S>
			State weightedSum(S f1, const State& s1, S f2, const State& s2) const {
				std::vector<S> factors;
				factors.push_back(f1);
				factors.push_back(f2);
				std::vector<State> states;
				states.push_back(s1);
				states.push_back(s2);
				return weightedSum(factors, states);
			}

			// TODO this is the default implementation
			template<typename S>
			State weightedSum(S f1, const State& s1, S f2, const State& s2, S f3, const State& s3) const {
				std::vector<S> factors;
				factors.push_back(f1);
				factors.push_back(f2);
				factors.push_back(f3);
				std::vector<State> states;
				states.push_back(s1);
				states.push_back(s2);
				states.push_back(s3);
				return weightedSum(factors, states);
			}

			template<typename S>
			State weightedSum(const std::vector<S>& factors, const std::vector<State>& states) const {
				INVALID_SIZE_EXCEPTION(factors.size() != states.size()) // Number of factors and states must be equal!
				return Danvil::SO3::RotationTools::WeightedMean(states, factors, (K)1e-3);
			}

		protected:
			~SO3Ops() {}
		};
	}

	//---------------------------------------------------------------------------

	namespace Domains
	{
		template<typename K>
		struct Full
		{
			typedef Danvil::SO3::Quaternion<K> State;

			Full() {}

			size_t dimension() const {
				return 3;
			}

			State zero() const {
				return State::Identity();
			}

			State unit(unsigned int k) const {
				return Danvil::SO3::ConvertToQuaternion(
						Danvil::SO3::AxisAngle<K>(
								Danvil::ctLinAlg::Vec3<K>::FactorUnit(k),
								K(1)));
			}

			template<typename SCL>
			State unit(unsigned int k, SCL s) const {
				return Danvil::SO3::ConvertToQuaternion(
						Danvil::SO3::AxisAngle<K>(
								Danvil::ctLinAlg::Vec3<K>::FactorUnit(k),
								K(s)));
			}

			State project(const State& s) const {
				State cp = s;
				cp.normalize();
				return cp;
			}

			State random() const {
				return Danvil::SO3::RotationTools::UniformRandom<K>(&RandomNumbers::Uniform<K>);
			}

			template<typename NT>
			State random(const State& center, const std::vector<NT>& noise) const {
				assert(noise.size() == dimension());
				// FIXME correct noise for SO3
				K d = noise[0];// K(std::sqrt(noise[0]*noise[0] + noise[1]*noise[1] + noise[2]*noise[2]));
				State delta = Danvil::SO3::RotationTools::UniformRandom<K>(d, &RandomNumbers::Uniform<K>);
				return delta * center;
			}

		protected:
			~Full() {}
		};

		template<typename K>
		struct MaxDistance
		{
			typedef Danvil::SO3::Quaternion<K> State;

			MaxDistance(K range = Danvil::Pi<K>())
			: range_(range) {}

			size_t dimension() const {
				return 3;
			}

			State project(const State& s) const {
				// use a quaternion which has at most an rotation angle of PI
				State r = (s.w >= 0) ? s : -s;
				double angle = 2 * std::acos(r.w); // is in [0,PI], because r.w > 0!
				if(angle < range_) {
					return r;
				} else {
					// reduce rotation by creating a rotation of maximal angle and given axis
					return State::FactorVectorAngle(r.x, r.y, r.z, range_);
				}
			}

			State random() const {
				return Danvil::SO3::RotationTools::UniformRandom<K>(range_, &RandomNumbers::Uniform<K>);
			}

			template<typename NT>
			State random(const State& center, const std::vector<NT>& noise) const {
				assert(noise.size() == dimension());
				// FIXME correct noise for SO3
				K d = K(std::sqrt(noise[0]*noise[0] + noise[1]*noise[1] + noise[2]*noise[2]));
				State s = center * Danvil::SO3::RotationTools::UniformRandom<K>(d, &RandomNumbers::Uniform<K>);
				return project(s);
			}

			DEFINE_FIELD(range, K)

		protected:
			~MaxDistance() {}
		};

		template<typename K>
		struct ConeTwist
		{
			typedef Danvil::SO3::Quaternion<K> State;

			typedef Danvil::ctLinAlg::Vec3<K> V3;

			struct ConeTwistParams {
				K twist;
				K cone_radius;
				K cone_phi;
			};

			static const V3 TwistAxis() {
				return V3(0, 1, 0);
			}

			static ConeTwistParams QuaternionToConeTwist(const State& q) {
				ConeTwistParams ctp;
				// compute a rotation which undoes the cone rotation with minimal angle
				V3 twist_axis = TwistAxis();
				V3 twist_axis_under_q = State::TransformVector(q, twist_axis);
				V3 cone_axis = Danvil::ctLinAlg::Cross(twist_axis, twist_axis_under_q);
				K cone_axis_len = Danvil::ctLinAlg::Length(cone_axis);
				State q_no_cone;
				if(cone_axis_len > K(0)) {
					cone_axis *= K(1) / cone_axis_len;
					// TODO use the fact that the length is equal to sin(undo_cone_angle)
					K cone_angle = Danvil::MoreMath::ProtectedAcos(twist_axis_under_q * twist_axis);
					// using negative axis gives undo rotation
					State q_undo_cone = Danvil::SO3::ConvertToQuaternion(Danvil::SO3::AxisAngle<K>(- cone_axis, cone_angle));
					// now we know the angle rotation
					ctp.cone_radius = cone_angle;
					ctp.cone_phi = std::atan2(cone_axis.z, cone_axis.x); // TODO only valid for twist axis (0,1,0)
					// undo cone rotation
					q_no_cone = q_undo_cone * q;
				}
				else {
					// no cone rotation
					ctp.cone_radius = K(0);
					ctp.cone_phi = K(0);
					q_no_cone = q;
				}
				// get twist angle
				K sin_tau_half = Danvil::ctLinAlg::Dot(q_no_cone.imaginary(), twist_axis);
				K cos_tau_half = q_no_cone.real();
				ctp.twist = 2.0f * std::atan2(sin_tau_half, cos_tau_half);
				Danvil::MoreMath::WrapInplace(ctp.twist, -K(Danvil::C_PI), +K(Danvil::C_PI));
				// ok...
				return ctp;
			}

			static State ConeTwistToQuaternion(const ConeTwistParams& ct) {
				V3 twist_axis = TwistAxis();
				State T = Danvil::SO3::ConvertToQuaternion(Danvil::SO3::AxisAngle<K>(twist_axis, ct.twist));
				V3 cone_axis(std::cos(ct.cone_phi), 0, std::sin(ct.cone_phi));
				State C = Danvil::SO3::ConvertToQuaternion(Danvil::SO3::AxisAngle<K>(cone_axis, ct.cone_radius));
				return C*T;
			}

			ConeTwist()
			: cone_max_(Danvil::C_PI), twist_max_(Danvil::C_PI) {}

			ConeTwist(K cone_max, K twist_max)
			: cone_max_(cone_max), twist_max_(twist_max) {}

			size_t dimension() const {
				// cone rotation has two degrees of freedom
				// twist rotation has one degree of freedom
				return 3;
			}
			State zero() const {
				return State::Identity();
			}

			State unit(unsigned int k) const {
				// TODO or use cone/twist parameters?
				return Danvil::SO3::ConvertToQuaternion(
						Danvil::SO3::AxisAngle<K>(
								Danvil::ctLinAlg::Vec3<K>::FactorUnit(k),
								K(1)));
			}

			template<typename SCL>
			State unit(unsigned int k, SCL s) const {
				// TODO or use cone/twist parameters?
				return Danvil::SO3::ConvertToQuaternion(
						Danvil::SO3::AxisAngle<K>(
								Danvil::ctLinAlg::Vec3<K>::FactorUnit(k),
								K(s)));
			}

			State project(const State& s) const {
				// compute cone twist parameters
				ConeTwistParams ctp = QuaternionToConeTwist(s);
				// clamp parameter to allowed interval
				ctp.cone_radius = std::min(ctp.cone_radius, cone_max_);
				ctp.twist = std::min(std::max(ctp.twist, -twist_max_), +twist_max_);
				// return quaternion
				return ConeTwistToQuaternion(ctp);
			}

			State random() const {
				ConeTwistParams ctp;
				// random twist
				ctp.twist = RandomNumbers::Uniform<K>(-twist_max_, +twist_max_);
				// random cone
				ctp.cone_radius = std::sqrt(RandomNumbers::Uniform<K>(K(0), cone_max_));
				ctp.cone_phi = RandomNumbers::Uniform<K>(K(0), K(Danvil::C_2_PI));
				// return quaternion
				return ConeTwistToQuaternion(ctp);
			}

			template<typename NT>
			State random(const State& center, const std::vector<NT>& noise) const {
				ConeTwistParams ctp = QuaternionToConeTwist(center);
				// get allowed range
				assert(noise.size() == dimension());
				K noise_alpha_r = noise[0];
				K noise_alpha_dir = noise[1];
				K noise_twist = noise[2];
				// random twist angle in range
				ctp.twist = RandomNumbers::Uniform(
						std::max(-twist_max_, ctp.twist - noise_twist),
						std::min(ctp.twist + noise_twist, twist_max_));
				// random cone rotation amount in range
				ctp.cone_radius = RandomNumbers::Uniform(
						std::max(K(0), std::min(ctp.cone_radius - noise_alpha_r, cone_max_)),
						std::min(ctp.cone_radius + noise_alpha_r, cone_max_));
				// random cone rotation direction in range
				ctp.cone_phi = RandomNumbers::Uniform(
						ctp.cone_phi - noise_alpha_dir,
						ctp.cone_phi + noise_alpha_dir);
				// return quaternion
				return ConeTwistToQuaternion(ctp);
			}

		private:
			K cone_max_;
		public:
			K cone_max() const {
				return cone_max_;
			}
			void set_cone_max(K x) {
				cone_max_ = x;
			}

		private:
			K twist_max_;
		public:
			K twist_max() const {
				return twist_max_;
			}
			void set_twist_max(K x) {
				twist_max_ = x;
			}

		protected:
			~ConeTwist() {}
		};
	}

	//---------------------------------------------------------------------------

	template<
		typename K,
		class Domain = Domains::Full<K>,
		class Operator = Operations::SO3Ops<K>,
		class OperationFinal = OperationFinalPolicy::Unprojected<Danvil::SO3::Quaternion<K> >
	>
	struct FullSO3Space
	: public BaseSpace<Danvil::SO3::Quaternion<K>, Operator, Domain, OperationFinal>
	{
		virtual void print(std::ostream& os) const {
			os << "FullSO3Space";
		}
	};

	template<
		typename K,
		class Domain = Domains::ConeTwist<K>,
		class Operator = Operations::SO3Ops<K>,
		class OperationFinal = OperationFinalPolicy::Projected<Danvil::SO3::Quaternion<K> >
	>
	struct ConeTwistSpace
	: public BaseSpace<Danvil::SO3::Quaternion<K>, Operator, Domain, OperationFinal>
	{
		virtual void print(std::ostream& os) const {
			os << "ConeTwistSpace";
		}
	};

}

//---------------------------------------------------------------------------
}}
//---------------------------------------------------------------------------
#endif
