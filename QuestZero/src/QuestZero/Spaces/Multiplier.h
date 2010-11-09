/*
 * Multiplier.h
 *
 *  Created on: Sep 11, 2010
 *      Author: david
 */

#ifndef MULTIPLIER_H_
#define MULTIPLIER_H_

#include "GetScalarType.h"
#include <Danvil/Print.h>
#include <boost/shared_array.hpp>
#include <vector>

namespace Spaces
{

namespace MultiplierSizePolicies
{
	template<int N> struct FixedSize
	{
		unsigned int count() const { return N; }
	};

	struct DynamicSize
	{
		DynamicSize(int count) : count_(count) {}
		unsigned int count() const { return count_; }
	private:
		unsigned int count_;
	};
}

template<typename BaseState, class SizePolicy_>
struct MultiplierState
: public Danvil::Print::IPrintable
{
	typedef typename Private::GetScalarType<BaseState>::ScalarType ScalarType;

	typedef SizePolicy_ SizePolicy;

	MultiplierState(SizePolicy sp=SizePolicy())
	: size_policy_(sp) {
		sub_ = boost::shared_array<BaseState>(new BaseState[count()]);
	}

	unsigned int count() const {
		return size_policy_.count();
	}

	SizePolicy size_policy() const {
		return size_policy_;
	}

	BaseState& operator[](int i) {
		return sub_[i];
	}

	const BaseState& operator[](int i) const {
		return sub_[i];
	}

	void print(std::ostream& os) const {
		os << "[";
		for(unsigned int i=0; i<count(); i++) {
			os << i << "=" << sub_[i];
			if(i != count()-1) {
				os << ", ";
			}
		}
		os << "]";
	}

private:
	SizePolicy size_policy_;
	boost::shared_array<BaseState> sub_;

};

template<typename BaseSpace, typename State_>
struct MultiplierSpace
{
	typedef State_ State;

	typedef typename State::SizePolicy SizePolicy;

	struct InvalidNoiseVectorException {};

	struct WeightedSumException {};

	MultiplierSpace(SizePolicy sp=SizePolicy())
	: size_policy_(sp) {
		spaces_ = boost::shared_array<BaseSpace>(new BaseSpace[count()]);
	}

	unsigned int count() const {
		return size_policy_.count();
	}

	BaseSpace& operator[](int i) {
		return spaces_[i];
	}

	const BaseSpace& operator[](int i) const {
		return spaces_[i];
	}

	unsigned int dimension() const {
		unsigned int n = 0;
		for(unsigned int i=0; i<count(); i++) {
			n += spaces_[i].dimension();
		}
		return n;
	}

	double distance(const State& a, const State& b) const {
		double d = 0;
		for(unsigned int i=0; i<count(); ++i) {
			d += spaces_[i].distance(a[i], b[i]);
		}
		return d;
	}

	State inverse(const State& a) const {
		State s;
		for(unsigned int i=0; i<count(); ++i) {
			s[i] = spaces_[i].inverse(a[i]);
		}
		return s;
	}

	State compose(const State& a, const State& b) const {
		State s;
		for(unsigned int i=0; i<count(); ++i) {
			s[i] = spaces_[i].compose(a[i], b[i]);
		}
		return s;
	}

	template<typename K>
	State weightedSum(K f1, const State& s1, K f2, const State& s2) const {
		State s;
		for(unsigned int i=0; i<count(); ++i) {
			s[i] = spaces_[i].weightedSum(f1, s1[i], f2, s2[i]);
		}
		return s;
	}

	template<typename K>
	State weightedSum(K f1, const State& s1, K f2, const State& s2, K f3, const State& s3) const {
		State s;
		for(unsigned int i=0; i<count(); ++i) {
			s[i] = spaces_[i].weightedSum(f1, s1[i], f2, s2[i], f3, s3[i]);
		}
		return s;
	}

	template<typename K>
	State weightedSum(const std::vector<K>& factors, const std::vector<State>& states) const {
		if(factors.size() != states.size()) {
			// Number of factors and states must be equal!
			throw WeightedSumException();
		}
		if(states.size() == 0) {
			// Must have at least one element for WeightedSum!
			throw WeightedSumException();
		}
		State s;
		for(unsigned int i=0; i<count(); ++i) {
			std::vector<State> parts;
			parts.reserve(states.size());
			for(size_t k=0; k<states.size(); k++) {
				parts[k] = states[k][i];
			}
			s[i] = weightedSum(factors, states);
		}
		return s;
	}

	State project(const State& s) const {
		State v;
		for(size_t i=0; i<count(); i++) {
			v[i] = spaces_[i].project(s[i]);
		}
		return v;
	}

	State random() const {
		State v;
		for(size_t i=0; i<count(); i++) {
			v[i] = spaces_[i].random();
		}
		return v;
	}

	template<typename K>
	State random(const State& center, const std::vector<K>& noise) const {
		State v;
		size_t start = 0;
		for(size_t i=0; i<count(); i++) {
			size_t len = spaces_[i].dimension();
			if(start + len > noise.size()) {
				throw InvalidNoiseVectorException();
			}
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

}

#endif
