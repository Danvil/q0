/*
 * TypelistSpace.h
 *
 *  Created on: Sep 11, 2010
 *      Author: david
 */

#ifndef TYPELISTSPACE_H_
#define TYPELISTSPACE_H_

#include "GetScalarType.h"
#include <Danvil/Print.h>
#include <loki/HierarchyGenerators.h>
#include <loki/Typelist.h>
#include <vector>

namespace Spaces {

template<class Typelist>
struct TypelistState
: public Loki::Tuple<Typelist>,
  Danvil::Print::IPrintable
{
	typedef typename Private::GetScalarType<typename Loki::TL::TypeAt<Typelist, 0>::Result>::ScalarType ScalarType;

	static const int N = Loki::TL::Length<Typelist>::value;

	// FIXME is i the type index or the global index?
	/** Valid indices reach are [1 | Length(Typelist)] */
	template<int i>
	const typename Loki::TL::TypeAt<Typelist, i>::Result& part() const {
		return Loki::Field<i>(*this);
	}

	template<int i>
	typename Loki::TL::TypeAt<Typelist, i>::Result& part() {
		return Loki::Field<i>(*this);
	}

	template<int i>
	void setPart(const typename Loki::TL::TypeAt<Typelist, i>::Result& part) {
		Loki::Field<i>(*this) = part;
	}

	unsigned int dimension() const {
		unsigned int sum = 0;
		dimensionImpl(Loki::Int2Type<0>(), sum);
		return sum;
	}

	void print(std::ostream& os) const {
		os << "[";
		printImpl(Loki::Int2Type<0>(), os);
		os << "]";
	}

private:
	template<int i>
	void dimensionImpl(Loki::Int2Type<i>, unsigned int& sum) const {
		sum += part<i>().dimension();
		dimensionImpl(Loki::Int2Type<i+1>(), sum);
	}

	void dimensionImpl(Loki::Int2Type<N>, unsigned int&) const {}

	template<int i>
	void printImpl(Loki::Int2Type<i>, std::ostream& os) const {
		os << i << "=" << part<i>();
		if(i != N - 1) {
			os << ", ";
		}
		printImpl(Loki::Int2Type<i+1>(), os);
	}

	void printImpl(Loki::Int2Type<N>, std::ostream&) const {}

};

template<class Typelist, class State_>
struct TypelistSpace
: public Loki::Tuple<Typelist>
{
	typedef State_ State;

	static const int N = Loki::TL::Length<Typelist>::value;

	struct InvalidNoiseVectorException {};

	// FIXME is i the type index or the global index?
	/** Valid indices reach are [1 | Length(Typelist)] */
	template<int i>
	const typename Loki::TL::TypeAt<Typelist, i>::Result& space() const {
		return Loki::Field<i>(*this);
	}

	template<int i>
	typename Loki::TL::TypeAt<Typelist, i>::Result& space() {
		return Loki::Field<i>(*this);
	}

	template<int i>
	void setSpace(const typename Loki::TL::TypeAt<Typelist, i>::Result& space) {
		Loki::Field<i>(*this) = space;
	}

	double distance(const TypelistState<Typelist>& a, const TypelistState<Typelist>& b) const {
		double d = 0;
		distanceImpl(Loki::Int2Type<0>(), d, a, b);
		return d;
	}

	State inverse(const State& a) const {
		State s;
		inverseImpl(Loki::Int2Type<0>(), s, a);
		return s;
	}

	State compose(const State& a, const State& b) const {
		State s;
		composeImpl(Loki::Int2Type<0>(), s, a, b);
		return s;
	}

	// TODO this is default
	State difference(const State& a, const State& b) const {
		return compose(a, inverse(b));
	}

	template<typename K>
	State weightedSum(K f1, const State& s1, K f2, const State& s2) const {
		State s;
		weightedSumImpl(Loki::Int2Type<0>(), s, f1, s1, f2, s2);
		return s;
	}

	template<typename K>
	State weightedSum(K f1, const State& s1, K f2, const State& s2, K f3, const State& s3) const {
		State s;
		weightedSumImpl(Loki::Int2Type<0>(), s, f1, s1, f2, s2, f3, s3);
		return s;
	}

	template<typename K>
	State weightedSum(const std::vector<K>& factors, const std::vector<State>& states) const {
		State s;
		weightedSumImpl(Loki::Int2Type<0>(), s, factors, states);
		return s;
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

	State random() const {
		State s;
		randomImpl(Loki::Int2Type<0>(), s);
		return s;
	}

	template<typename K>
	State random(const State& center, const std::vector<K>& noise) const {
		State s;
		randomImpl(Loki::Int2Type<0>(), s, 0, center, noise);
		return s;
	}

	State project(const State& x) const {
		State s;
		projectImpl(Loki::Int2Type<0>(), s, x);
		return s;
	}

private:
	template<int i>
	void distanceImpl(Loki::Int2Type<i>, double& d, const State& a, const State& b) const {
		d += space<i>().distance(a.template part<i>(), b.template part<i>());
		differenceImpl(Loki::Int2Type<i+1>(), d, a, b);
	}

	void distanceImpl(Loki::Int2Type<N>, double&, const State&, const State&) const {}

	template<int i>
	void inverseImpl(Loki::Int2Type<i>, State& s, const State& a) const {
		s.template setPart<i>(space<i>().inverse(a.template part<i>()));
		inverseImpl(Loki::Int2Type<i+1>(), s, a);
	}

	void inverseImpl(Loki::Int2Type<N>, State&, const State&) const {}

	template<int i>
	void composeImpl(Loki::Int2Type<i>, State& s, const State& a, const State& b) const {
		s.template setPart<i>(space<i>().compose(a.template part<i>(), b.template part<i>()));
		composeImpl(Loki::Int2Type<i+1>(), s, a, b);
	}

	void composeImpl(Loki::Int2Type<N>, State&, const State&, const State&) const {}

	template<typename K, int i>
	void weightedSumImpl(Loki::Int2Type<i>, State& s, K f1, const State& s1, K f2, const State& s2) const {
		s.template setPart<i>(space<i>().weightedSum(f1, s1.template part<i>(), f2, s2.template part<i>()));
		weightedSumImpl(Loki::Int2Type<i+1>(), s, f1, s1, f2, s2);
	}

	template<typename K>
	void weightedSumImpl(Loki::Int2Type<N>, State& s, K, const State&, K, const State&) const {}

	template<typename K, int i>
	void weightedSumImpl(Loki::Int2Type<i>, State& s, K f1, const State& s1, K f2, const State& s2, K f3, const State& s3) const {
		s.template setPart<i>(space<i>().weightedSum(f1, s1.template part<i>(), f2, s2.template part<i>(), f3, s3.template part<i>()));
		weightedSumImpl(Loki::Int2Type<i+1>(), s, f1, s1, f2, s2, f3, s3);
	}

	template<typename K>
	void weightedSumImpl(Loki::Int2Type<N>, State& s, K, const State&, K, const State&, K, const State&) const {}

	template<typename K, int i>
	void weightedSumImpl(Loki::Int2Type<i>, State& s, const std::vector<K>& scalars, const std::vector<State>& states) const {
		typedef typename Loki::TL::TypeAt<Typelist, i - 1>::Result CurrentType;
		std::vector<CurrentType> parts;
		parts.reserve(states.size());
		for(size_t j=0; j<states.size(); ++j) {
			parts[j] = states[j].template part<i>();
		}
		s.template setPart<i>(space<i>().weightedSum(scalars, parts));
		weightedSumImpl(Loki::Int2Type<i+1>(), s, scalars, states);
	}

	template<typename K>
	void weightedSumImpl(Loki::Int2Type<N>, State& s, const std::vector<K>&, const std::vector<State>&) const {}

	template<int i>
	void randomImpl(Loki::Int2Type<i>, State& s) const {
		s.template setPart<i>(space<i>().random());
		randomImpl(Loki::Int2Type<i+1>(), s);
	}

	void randomImpl(Loki::Int2Type<N>, State&) const {}

	template<typename K, int i>
	void randomImpl(Loki::Int2Type<i>, State& s, unsigned int start, const State& center, const std::vector<K>& noise) const {
		unsigned int len = space<i>().dimension();
		unsigned int end = start + len;
		if(end > noise.size()) {
			throw InvalidNoiseVectorException();
		}
		s.template setPart<i>(space<i>().random(center.template part<i>(), std::vector<K>(noise.begin() + start, noise.begin() + end)));
		randomImpl(Loki::Int2Type<i+1>(), s, end, center, noise);
	}

	template<typename K>
	void randomImpl(Loki::Int2Type<N>, State&, unsigned int, const State&, const std::vector<K>&) const {}

	template<int i>
	void projectImpl(Loki::Int2Type<i>, State& s, const State& a) const {
		s.template setPart<i>(space<i>().project(a.template part<i>()));
		projectImpl(Loki::Int2Type<i+1>(), s, a);
	}

	void projectImpl(Loki::Int2Type<N>, State&, const State&) const {}

};


}

#endif
