/*
 * Multiplier.h
 *
 *  Created on: Sep 11, 2010
 *      Author: david
 */

#ifndef MULTIPLIER_H_
#define MULTIPLIER_H_
//---------------------------------------------------------------------------
#include "GetScalarType.h"
#include "QuestZero/Common/Exceptions.h"
#include <Danvil/Print.h>
#include <Danvil/Tools/Debug.h>
#include <boost/shared_array.hpp>
#include <vector>
//---------------------------------------------------------------------------
namespace Q0 {
namespace Spaces {
//---------------------------------------------------------------------------

namespace MultiplierSizePolicies
{
	template<int N> struct FixedSize
	{
		FixedSize() {}
		FixedSize(int count) { assert(count==N); }
		unsigned int count() const { return N; }
	public:
		template<typename U>
		struct StorageTraits {
			typedef U Storage[N];
			static void Allocate(Storage&, uint n) { assert(n == N); }
		};
	};

	struct DynamicSize
	{
		DynamicSize() : count_(0) {}
		DynamicSize(int count) : count_(count) {}
		DynamicSize(const DynamicSize& rhs) : count_(rhs.count_) {}
		DynamicSize& operator=(const DynamicSize& rhs) { count_ = rhs.count_; return *this; }
		unsigned int count() const { return count_; }
	private:
		unsigned int count_;
	public:
		template<typename U>
		struct StorageTraits {
			typedef boost::shared_array<U> Storage;
			static void Allocate(Storage& s, uint n) {
				s = boost::shared_array<U>(new U[n]);
			}
		};
	};
}

template<typename BaseState_, class SizePolicy_=MultiplierSizePolicies::DynamicSize>
struct MultiplierState
: public Danvil::Print::IPrintable
{
	typedef SizePolicy_ SizePolicy;

	typedef BaseState_ BaseState;

	typedef typename Private::GetScalarType<BaseState>::ScalarType ScalarType;

	MultiplierState() {
		// size_policy_ has default value
		allocate();
	}

	MultiplierState(SizePolicy sp)
	: size_policy_(sp) {
		allocate();
	}

	MultiplierState(const MultiplierState& rhs)
	: size_policy_(rhs.size_policy_) {
		allocate();
		copy_from(rhs.sub_);
	}

	MultiplierState& operator=(const MultiplierState& rhs) {
		if(&rhs != this) {
			size_policy_ = rhs.size_policy_;
			allocate();
			copy_from(rhs.sub_);
		}
		return *this;
	}

	unsigned int count() const {
		return size_policy_.count();
	}

	unsigned int dimension() const {
		unsigned int d = 0;
		for(size_t i=0; i<count(); ++i) {
			d += sub_[i].dimension();
		}
		return 0;
	}

	BaseState& operator[](size_t i) {
		DEBUG_ASSERT_MESSAGE(i < count(), "Index out of bound");
		return sub_[i];
	}

	const BaseState& operator[](size_t i) const {
		DEBUG_ASSERT_MESSAGE(i < count(), "Index out of bound");
		return sub_[i];
	}

	void print(std::ostream& os) const {
		os << "[X: ";
		for(size_t i=0; i<count(); ++i) {
			//os << i << "=" << sub_[i];
			os << sub_[i];
			if(i != count() - 1) {
				os << ", ";
			}
		}
		os << "]";
	}

private:
	void allocate() {
		SizePolicy::template StorageTraits<BaseState>::Allocate(sub_, count());
		// sub_ = boost::shared_array<BaseState>(new BaseState[count()]);
	}

	void copy_from(const typename SizePolicy::template StorageTraits<BaseState>::Storage& data) {
		for(size_t i=0; i<count(); i++) {
			sub_[i] = data[i];
		}
	}

private:
	typename SizePolicy::template StorageTraits<BaseState>::Storage sub_;
	//boost::shared_array<BaseState> sub_;
	SizePolicy size_policy_;

};

template<typename BaseSpace, typename State_, typename BT=typename State_::BaseState>
struct MultiplierSpace
{
	typedef State_ State;

	typedef typename State::BaseState BaseState;

	typedef typename State::SizePolicy SizePolicy;

	MultiplierSpace(SizePolicy sp=SizePolicy())
	: size_policy_(sp) {
		spaces_ = boost::shared_array<BaseSpace>(new BaseSpace[count()]);
	}

	/** Number of parts */
	unsigned int count() const {
		return size_policy_.count();
	}

	BaseSpace& operator[](size_t i) {
		DEBUG_ASSERT_MESSAGE(i < count(), "Part index out of bound!");
		return spaces_[i];
	}

	const BaseSpace& operator[](size_t i) const {
		DEBUG_ASSERT_MESSAGE(i < count(), "Part index out of bound!");
		return spaces_[i];
	}

	/** Dimension of this space which is the sum of all part dimensions
	 * Same as GetSummedDimension(count())
	 */
	unsigned int dimension() const {
		unsigned int n = 0;
		for(size_t i=0; i<count(); i++) {
			n += GetPartDimension(i);
		}
		return n;
	}

	/** Gets the dimension of a part */
	size_t GetPartDimension(size_t part) const {
		return (*this)[part].dimension();
	}

	/** Sum of dimensions over all prior parts
	 * This is necessary as the part dimensions must not be equal.
	 */
	size_t GetSummedDimension(size_t part) const {
		DEBUG_ASSERT_MESSAGE(part < count(), "Part index out of bound!");
		size_t p = 0;
		for(size_t i=0; i<part; i++) {
			p += GetPartDimension(i);
		}
		return p;
	}

	double distance(const State& a, const State& b) const {
		double d = 0;
		for(size_t i=0; i<count(); ++i) {
			d += spaces_[i].distance(a[i], b[i]);
		}
		return d;
	}

	State scale(const State& a, double scl) const {
		State s(size_policy_);
		for(size_t i=0; i<count(); ++i) {
			s[i] = spaces_[i].scale(a[i], scl);
		}
		return s;
	}

	State inverse(const State& a) const {
		State s(size_policy_);
		for(size_t i=0; i<count(); ++i) {
			s[i] = spaces_[i].inverse(a[i]);
		}
		return s;
	}

	State compose(const State& a, const State& b) const {
		State s(size_policy_);
		for(size_t i=0; i<count(); ++i) {
			s[i] = spaces_[i].compose(a[i], b[i]);
		}
		return s;
	}

	template<typename K>
	State weightedSum(K f1, const State& s1, K f2, const State& s2) const {
		State s(size_policy_);
		for(size_t i=0; i<count(); ++i) {
			s[i] = spaces_[i].weightedSum(f1, s1[i], f2, s2[i]);
		}
		return s;
	}

	template<typename K>
	State weightedSum(K f1, const State& s1, K f2, const State& s2, K f3, const State& s3) const {
		State s(size_policy_);
		for(size_t i=0; i<count(); ++i) {
			s[i] = spaces_[i].weightedSum(f1, s1[i], f2, s2[i], f3, s3[i]);
		}
		return s;
	}

	template<typename K>
	State weightedSum(const std::vector<K>& factors, const std::vector<State>& states) const {
		INVALID_SIZE_EXCEPTION(factors.size() != states.size()) // Number of factors and states must be equal!
		INVALID_SIZE_EXCEPTION(states.size() == 0) // Must have at least one element for WeightedSum!
		State s(size_policy_);
		for(size_t i=0; i<count(); ++i) {
			std::vector<BT> parts;
			parts.reserve(states.size());
			for(size_t k=0; k<states.size(); k++) {
				parts.push_back(states[k][i]);
			}
			s[i] = spaces_[i].weightedSum(factors, parts);
		}
		return s;
	}

	State zero() const {
		State v(size_policy_);
		for(size_t i=0; i<count(); i++) {
			v[i] = spaces_[i].zero();
		}
		return v;
	}

	State unit(unsigned int k) const {
		State v(size_policy_);
		unsigned int p = 0;
		for(size_t i=0; i<count(); i++) {
			unsigned int len = spaces_[i].dimension();
			if(p <= k && k < p + len) {
				v[i] = spaces_[i].unit(k - p);
			}
			else {
				v[i] = spaces_[i].zero();
			}
			p += len;
		}
		return v;
	}

	template<typename SCL>
	State unit(unsigned int k, SCL s) const {
		State v(size_policy_);
		unsigned int p = 0;
		for(size_t i=0; i<count(); i++) {
			unsigned int len = spaces_[i].dimension();
			if(p <= k && k < p + len) {
				v[i] = spaces_[i].unit(k - p, s);
			}
			else {
				v[i] = spaces_[i].zero();
			}
			p += len;
		}
		return v;
	}

	State project(const State& s) const {
		State v(size_policy_);
		for(size_t i=0; i<count(); i++) {
			v[i] = spaces_[i].project(s[i]);
		}
		return v;
	}

	State random() const {
		State v(size_policy_);
		for(size_t i=0; i<count(); i++) {
			v[i] = spaces_[i].random();
		}
		return v;
	}

	template<typename K>
	State random(const State& center, const std::vector<K>& noise) const {
		State v(size_policy_);
		size_t start = 0;
		for(size_t i=0; i<count(); i++) {
			size_t len = spaces_[i].dimension();
			INVALID_SIZE_EXCEPTION(start + len > noise.size())
			v[i] = spaces_[i].random(center[i], std::vector<K>(noise.begin() + start, noise.begin() + start + len));
			start += len;
		}
		return v;
	}

	// TODO this is default
	State difference(const State& a, const State& b) const {
		return compose(a, inverse(b));
	}

	// TODO this is default
	State mean(const std::vector<State>& states) const {
		typedef typename State::ScalarType Scalar;
		return weightedSum(std::vector<Scalar>(states.size(), (Scalar)1), states);
	}

	// TODO this is default
	std::vector<State> randomMany(size_t n) const {
		std::vector<State> states;
		for(size_t i=0; i<n; i++) {
			states.push_back(this->random());
		}
		return states;
	}

private:
	SizePolicy size_policy_;
	boost::shared_array<BaseSpace> spaces_;

};

//---------------------------------------------------------------------------
}}
//---------------------------------------------------------------------------
#endif
