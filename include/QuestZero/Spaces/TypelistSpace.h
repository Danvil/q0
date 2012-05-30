/*
 * TypelistSpace.h
 *
 *  Created on: Sep 11, 2010
 *      Author: david
 */

#ifndef TYPELISTSPACE_H_
#define TYPELISTSPACE_H_
//---------------------------------------------------------------------------
#include "GetScalarType.h"
#include "QuestZero/Common/Exceptions.h"
#include <Danvil/Print.h>
#include <loki/HierarchyGenerators.h>
#include <loki/Typelist.h>
#include <vector>
//---------------------------------------------------------------------------
namespace Q0 {
namespace Spaces {
//---------------------------------------------------------------------------

/*namespace Part {
	struct _0 {};
	struct _1 {};
	struct _2 {};
	struct _3 {};
	struct _4 {};
	struct _5 {};
	struct _6 {};
	struct _7 {};
	struct _8 {};
	struct _9 {};
}*/

template<class Typelist_>
struct TypelistState
: public Loki::Tuple<Typelist_>,
  public Danvil::Print::IPrintable
{
	typedef Typelist_ Typelist;

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
	void set_part(const typename Loki::TL::TypeAt<Typelist, i>::Result& x) {
		Loki::Field<i>(*this) = x;
	}

/*#define Q0_GET_PART(i) \
	const typename Loki::TL::TypeAt<Typelist, i>::Result& part(Part::_##i) const { \
		return part<i>();\
	}\
	typename Loki::TL::TypeAt<Typelist, i>::Result& part(Part::_##i) { \
		return part<i>();\
	}

	Q0_GET_PART(0)
	Q0_GET_PART(1)
	Q0_GET_PART(2)
	Q0_GET_PART(3)
	Q0_GET_PART(4)
	Q0_GET_PART(5)
	Q0_GET_PART(6)
	Q0_GET_PART(7)
	Q0_GET_PART(8)
	Q0_GET_PART(9)

#undef Q0_GET_PART

#define Q0_SET_PART(i) \
	void set_part(Part::_##i, const typename Loki::TL::TypeAt<Typelist, i>::Result& x) {\
		set_part<i>(x);\
	}

	Q0_SET_PART(0)
	Q0_SET_PART(1)
	Q0_SET_PART(2)
	Q0_SET_PART(3)
	Q0_SET_PART(4)
	Q0_SET_PART(5)
	Q0_SET_PART(6)
	Q0_SET_PART(7)
	Q0_SET_PART(8)
	Q0_SET_PART(9)

#undef Q0_SET_PART*/

	unsigned int dimension() const {
		unsigned int sum = 0;
		dimensionImpl(Loki::Int2Type<0>(), sum);
		return sum;
	}

	void print(std::ostream& os) const {
		os << "[TL: ";
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
		//os << i << "=" << part<i>();
		os << part<i>();
		if(i != N - 1) {
			os << ", ";
		}
		printImpl(Loki::Int2Type<i+1>(), os);
	}

	void printImpl(Loki::Int2Type<N>, std::ostream&) const {}

};

template<class Typelist_, class State_>
struct TypelistSpace
: public Loki::Tuple<Typelist_>,
  public Danvil::Print::IPrintable
{
	typedef State_ State;
	typedef Typelist_ Typelist;

	typedef Typelist SpaceTypelist;
	typedef typename State::Typelist StateTypelist;

	static const int N = Loki::TL::Length<Typelist>::value;

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

	void print(std::ostream& os) const {
		os << "[TL: ";
		printImpl(Loki::Int2Type<0>(), os);
		os << "]";
	}

	unsigned int dimension() const {
		unsigned int sum = 0;
		dimensionImpl(Loki::Int2Type<0>(), sum);
		return sum;
	}

	double distance(const TypelistState<Typelist>& a, const TypelistState<Typelist>& b) const {
		double d = 0;
		distanceImpl(Loki::Int2Type<0>(), d, a, b);
		return d;
	}

	State scale(const State& a, double scl) const {
		State s;
		scaleImpl(Loki::Int2Type<0>(), s, a, scl);
		return s;
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

	State zero() const {
		State s;
		zeroImpl(Loki::Int2Type<0>(), s);
		return s;
	}

	State unit(unsigned int k) const {
		State s;
		unitImpl(Loki::Int2Type<0>(), s, 0, k);
		return s;
	}

	template<typename SCL>
	State unit(size_t k, SCL s) const {
		State state;
		unitSclImpl(Loki::Int2Type<0>(), state, 0, k, s);
		return state;
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
	void printImpl(Loki::Int2Type<i>, std::ostream& os) const {
		os << space<i>();
		if(i != N - 1) {
			os << ", ";
		}
		printImpl(Loki::Int2Type<i+1>(), os);
	}

	void printImpl(Loki::Int2Type<N>, std::ostream&) const {}

	template<int i>
	void dimensionImpl(Loki::Int2Type<i>, unsigned int& sum) const {
		sum += space<i>().dimension();
		dimensionImpl(Loki::Int2Type<i+1>(), sum);
	}

	void dimensionImpl(Loki::Int2Type<N>, unsigned int&) const {}

	template<int i>
	void distanceImpl(Loki::Int2Type<i>, double& d, const State& a, const State& b) const {
		d += space<i>().distance(a.template part<i>(), b.template part<i>());
		differenceImpl(Loki::Int2Type<i+1>(), d, a, b);
	}

	void distanceImpl(Loki::Int2Type<N>, double&, const State&, const State&) const {}

	template<int i>
	void scaleImpl(Loki::Int2Type<i>, State& s, const State& a, double scl) const {
		s.template set_part<i>(space<i>().scale(a.template part<i>(), scl));
		scaleImpl(Loki::Int2Type<i+1>(), s, a, scl);
	}

	void scaleImpl(Loki::Int2Type<N>, State&, const State&, double) const {}

	template<int i>
	void inverseImpl(Loki::Int2Type<i>, State& s, const State& a) const {
		s.template set_part<i>(space<i>().inverse(a.template part<i>()));
		inverseImpl(Loki::Int2Type<i+1>(), s, a);
	}

	void inverseImpl(Loki::Int2Type<N>, State&, const State&) const {}

	template<int i>
	void composeImpl(Loki::Int2Type<i>, State& s, const State& a, const State& b) const {
		s.template set_part<i>(space<i>().compose(a.template part<i>(), b.template part<i>()));
		composeImpl(Loki::Int2Type<i+1>(), s, a, b);
	}

	void composeImpl(Loki::Int2Type<N>, State&, const State&, const State&) const {}

	template<typename K, int i>
	void weightedSumImpl(Loki::Int2Type<i>, State& s, K f1, const State& s1, K f2, const State& s2) const {
		s.template set_part<i>(space<i>().weightedSum(f1, s1.template part<i>(), f2, s2.template part<i>()));
		weightedSumImpl(Loki::Int2Type<i+1>(), s, f1, s1, f2, s2);
	}

	template<typename K>
	void weightedSumImpl(Loki::Int2Type<N>, State& s, K, const State&, K, const State&) const {}

	template<typename K, int i>
	void weightedSumImpl(Loki::Int2Type<i>, State& s, K f1, const State& s1, K f2, const State& s2, K f3, const State& s3) const {
		s.template set_part<i>(space<i>().weightedSum(f1, s1.template part<i>(), f2, s2.template part<i>(), f3, s3.template part<i>()));
		weightedSumImpl(Loki::Int2Type<i+1>(), s, f1, s1, f2, s2, f3, s3);
	}

	template<typename K>
	void weightedSumImpl(Loki::Int2Type<N>, State& s, K, const State&, K, const State&, K, const State&) const {}

	template<typename K, int i>
	void weightedSumImpl(Loki::Int2Type<i>, State& s, const std::vector<K>& scalars, const std::vector<State>& states) const {
		typedef typename Loki::TL::TypeAt<StateTypelist, i>::Result CurrentType;
		std::vector<CurrentType> parts;
		parts.reserve(states.size());
		for(size_t j=0; j<states.size(); ++j) {
			parts.push_back(states[j].template part<i>());
		}
		s.template set_part<i>(space<i>().weightedSum(scalars, parts));
		weightedSumImpl(Loki::Int2Type<i+1>(), s, scalars, states);
	}

	template<typename K>
	void weightedSumImpl(Loki::Int2Type<N>, State& s, const std::vector<K>&, const std::vector<State>&) const {}

	template<int i>
	void zeroImpl(Loki::Int2Type<i>, State& s) const {
		s.template set_part<i>(space<i>().zero());
		zeroImpl(Loki::Int2Type<i+1>(), s);
	}

	void zeroImpl(Loki::Int2Type<N>, State&) const {}

	template<int i>
	void unitImpl(Loki::Int2Type<i>, State& s, unsigned int start, unsigned int k) const {
		unsigned int len = space<i>().dimension();
		if(start <= k && k < start + len) {
			s.template set_part<i>(space<i>().unit(k - start));
		}
		else {
			s.template set_part<i>(space<i>().zero());
		}
		unitImpl(Loki::Int2Type<i+1>(), s, start + len, k);
	}

	void unitImpl(Loki::Int2Type<N>, State&, unsigned int, unsigned int) const {}

	template<typename SCL, int i>
	void unitSclImpl(Loki::Int2Type<i>, State& s, unsigned int start, unsigned int k, SCL scl) const {
		unsigned int len = space<i>().dimension();
		if(start <= k && k < start + len) {
			s.template set_part<i>(space<i>().unit(k - start, scl));
		}
		else {
			s.template set_part<i>(space<i>().zero());
		}
		unitSclImpl(Loki::Int2Type<i+1>(), s, start + len, k, scl);
	}

	template<typename SCL>
	void unitSclImpl(Loki::Int2Type<N>, State&, unsigned int, unsigned int, SCL) const {}

	template<int i>
	void randomImpl(Loki::Int2Type<i>, State& s) const {
		s.template set_part<i>(space<i>().random());
		randomImpl(Loki::Int2Type<i+1>(), s);
	}

	void randomImpl(Loki::Int2Type<N>, State&) const {}

	template<typename K, int i>
	void randomImpl(Loki::Int2Type<i>, State& s, unsigned int start, const State& center, const std::vector<K>& noise) const {
		unsigned int len = space<i>().dimension();
		unsigned int end = start + len;
		INVALID_SIZE_EXCEPTION(end > noise.size())
		s.template set_part<i>(space<i>().random(center.template part<i>(), std::vector<K>(noise.begin() + start, noise.begin() + end)));
		randomImpl(Loki::Int2Type<i+1>(), s, end, center, noise);
	}

	template<typename K>
	void randomImpl(Loki::Int2Type<N>, State&, unsigned int, const State&, const std::vector<K>&) const {}

	template<int i>
	void projectImpl(Loki::Int2Type<i>, State& s, const State& a) const {
		s.template set_part<i>(space<i>().project(a.template part<i>()));
		projectImpl(Loki::Int2Type<i+1>(), s, a);
	}

	void projectImpl(Loki::Int2Type<N>, State&, const State&) const {}

};

//---------------------------------------------------------------------------
}}
//---------------------------------------------------------------------------
#endif