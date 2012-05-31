/*
 * Cone.h
 *
 *  Created on: Sep 12, 2011
 *      Author: david
 */

#ifndef SPACE_CONE_H_
#define SPACE_CONE_H_
//---------------------------------------------------------------------------
#include "BaseSpace.h"
#include "QuestZero/Common/RandomNumbers.h"
#include "QuestZero/Common/Exceptions.h"
#include <Danvil/LinAlg.h>
#include <Danvil/SO3.h>
#include <Danvil/Tools/MoreMath.h> // FIXME remove
#include <Danvil/Tools/Constants.h> // FIXME remove
#include <vector>
#include <cassert>
#include <cmath>
//---------------------------------------------------------------------------
namespace Q0 {
namespace Spaces {
//---------------------------------------------------------------------------

/** Operations and domains for a 2D rotation
 * The state vector is expected to be a floating point number.
 */
namespace Cone
{

	/** The cone joint is parameterized by two variables: rotation direction and rotation amount
	 * These describe a point on the tangent unit disc of (1,0,0),
	 * which itself is the axis and angle description of the rotation.
	 * This is a subspace of SO(3), thus operations are defined by SO(3).
	 */
	template<typename K>
	struct State
	{
		typedef Danvil::SO3::Quaternion<K> Q;

		State() {}

		State(K direction, K amount) : phi_(direction), r_(amount) {
			toQuaternion();
		}

		State(const Q& q) : q_(q) {
			fromQuaternion();
		}

		K direction() const { return phi_; }

		K amount() const { return r_; }

		Q quaternion() const { return q_; }

		Danvil::ctLinAlg::Vec2<K> discPoint() const {
			return Danvil::ctLinAlg::Vec2<K>(r_ * std::cos(phi_), r_ * std::sin(phi_));
		}

		static double distance(const State& a, const State& b) {
			return Q::Distance(a.quaternion(), b.quaternion());
		}

		State scaled(K s) const {
			// scale amount
			return State(direction(), s*amount());
		}

		State inverted() const {
			// invert direction
			return State(-direction(), amount());
		}

		static State mult(const State& a, const State& b) {
			return State(a.quaternion() * b.quaternion());
		}

		static State diff(const State& a, const State& b) {
			return mult(a, b.inverted());
		}

		template<typename L>
		static State weightedMean(const std::vector<L>& factors, const std::vector<State>& states) {
			Danvil::ctLinAlg::Vec2<K> center = Danvil::ctLinAlg::Vec2<K>::Zero();
			INVALID_SIZE_EXCEPTION(factors.size() != states.size()); // Number of factors and states must be equal!
			// build the mean in the tangent space
			// FIXME probably not correct ....
			for(size_t i=0; i<factors.size(); i++) {
				center += factors[i] * states[i].discPoint();
			}
			K direction = std::atan2(center.y, center.x);
			K amount = center.length();
			return State(direction, amount);
		}

	private:
		State(K direction, K amount, const Q& q) : phi_(direction), r_(amount), q_(q) {}

		void toQuaternion() {
			K h = std::sin(r_ / K(2));
			q_ = Q(std::cos(r_ / K(2)), K(0), h*std::cos(phi_), h*std::sin(phi_));
		}

		void fromQuaternion() {
			if(!Danvil::MoreMath::IsZero(q_.x())) {
				throw "Q0::Spaces::Cone: quaternion is not of the correct form";
			}
			r_ = (q_.w >= K(1)) ? K(0) : (K(2) * std::acos(q_.w)); // precision stuff
			phi_ = std::atan2(q_.z, q_.y);
		}

	private:
		K phi_;
		K r_;
		Q q_;
	};

	template<typename K>
	struct Operations
	{
		typedef State<K> S;

		struct WeightedSumException {};

		double distance(const S& a, const S& b) const {
			return S::distance(a, b);
		}

		S scale(const S& a, double s) const {
			return a.scaled(s);
		}

		S inverse(const S& a) const {
			return a.inverted();
		}

		S compose(const S& a, const S& b) const {
			return S::mult(a, b);
		}

		S difference(const S& a, const S& b) const {
			return S::diff(a, b);
		}

		template<typename L>
		S weightedSum(L f1, const S& s1, L f2, const S& s2) const {
			std::vector<L> factors;
			factors.push_back(f1);
			factors.push_back(f2);
			std::vector<S> states;
			states.push_back(s1);
			states.push_back(s2);
			return weightedSum(factors, states);
		}

		template<typename L>
		S weightedSum(L f1, const S& s1, L f2, const S& s2, L f3, const S& s3) const {
			std::vector<L> factors;
			factors.push_back(f1);
			factors.push_back(f2);
			factors.push_back(f3);
			std::vector<S> states;
			states.push_back(s1);
			states.push_back(s2);
			states.push_back(s3);
			return weightedSum(factors, states);
		}

		template<typename L>
		S weightedSum(const std::vector<L>& factors, const std::vector<S>& states) const {
			return S::weightedMean(factors, states);
		}

	protected:
		~Operations() {}
	};

	// TODO: allow states with variable dimension?
	template<typename K>
	struct Domain
	{
		typedef State<K> S;

		Domain() : amount_max_(Danvil::C_PI) {}

		Domain(K max) : amount_max_(max) {}

		size_t dimension() const {
			return 2;
		}

		S zero() const {
			return State(K(0), K(0));
		}

		S unit(size_t i) const {
			// FIXME or what?
			if(i == 0) {
				return S(K(1), K(0));
			}
			else {
				return S(K(0), K(1));
			}
		}

		template<typename SCL>
		S unit(size_t i, SCL s) const {
			if(i == 0) {
				return S(K(s), K(0));
			}
			else {
				return S(K(0), K(s));
			}
		}

		S project(const S& s) const {
			K amount = Danvil::MoreMath::Wrap(s.amount(), K(Danvil::C_2_PI));
			return S(s.direction(), std::min(amount, amount_max_));
		}

		S random() const {
			// random point on unit disc with radius amount_max_
			// direction is random angle
			K direction = K(Danvil::C_2_PI) * RandoNumbers::Uniform<K>();
			// amount is random radius (must use sqrt for uniform sampling)
			K amount = amount_max_ * std::sqrt(RandoNumbers::Uniform<K>());
			return S(direction, amout);;
		}

		template<typename NT>
		S random(const S& center, const std::vector<NT>& noise) const {
			std::assert(noise.size() == dimension());
			K direction_noise = noise[0];
			K amount_noise = noise[1];
			// randomly add to direction
			K direction = center.direction() + RandomNumbers::UniformMP(direction_noise);
			// random radius in bounds
			K amount = RandomNumbers::Uniform(
					std::max(K(0), center.amount() - amount_noise),
					std::min(center.amount() + amount_noise, amount_max_));
			return S(direction, amount);
		}

		/** maximal allowed rotation */
		K amount_max_;

	protected:
		~Domain() {}
	};

	template<typename K>
	struct ConeSpace
	: public BaseSpace<State, Operations<K>, Domain<K> >
	{ };

	typedef ConeSpace<float> ConeSpaceF;
	typedef ConeSpace<double> ConeSpaceD;

}

//---------------------------------------------------------------------------
}}
//---------------------------------------------------------------------------
#endif
