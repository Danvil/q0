/*
 * Cartesian.h
 *
 *  Created on: Sep 10, 2010
 *      Author: david
 */

#ifndef SPACE_CARTESIAN_H_
#define SPACE_CARTESIAN_H_
//---------------------------------------------------------------------------
#include "BaseSpace.h"
#include "QuestZero/Common/RandomNumbers.h"
#include <Danvil/LinAlg.h>
#include <Danvil/Memops/Copy.h>
#include <vector>
#include <cassert>
//---------------------------------------------------------------------------
namespace Spaces {
//---------------------------------------------------------------------------

namespace Cartesian {

	namespace Operations
	{
		template<typename State, typename S = typename State::ScalarType>
		struct Linear
		{
			struct WeightedSumException {};

			double distance(const State& a, const State& b) const {
				return (double)Danvil::ctLinAlg::Distance(a, b);
			}

			State inverse(const State& a) const {
				return -a;
			}

			State compose(const State& a, const State& b) const {
				return a + b;
			}

			State difference(const State& a, const State& b) const {
				return compose(a, inverse(b));
			}

			template<typename K>
			State weightedSum(K f1, const State& s1, K f2, const State& s2) const {
				return (S)f1 * s1 + (S)f2 * s2;
			}

			template<typename K>
			State weightedSum(K f1, const State& s1, K f2, const State& s2, K f3, const State& s3) const {
				return (S)f1 * s1 + (S)f2 * s2 + (S)f3 * s3;
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
				State c = factors[0] * states[0];
				for(size_t i=1; i<states.size(); i++) {
					c += (S)factors[i] * states[i];
				}
				return c;
			}

		protected:
			~Linear() {}
		};
	}

	//---------------------------------------------------------------------------

	namespace Domains
	{
		// TODO: allow states with variable dimension?
		template<typename State>
		struct Box
		{
			typedef double K;

			struct InvalidNoiseVectorException {};

			Box() {}

			Box(const State& min, const State& max)
			: _min(min),
			  _max(max) {}

			void setDomainRange(const State& range) {
				_min = -range;
				_max = range;
			}

			void setDomainMin(const State& min) {
				_min = min;
			}

			void setDomainMax(const State& max) {
				_max = max;
			}

			size_t dimension() const {
				return State::Dimension;
			}

			State project(const State& s) const {
				State v;
				for(size_t i=0; i<dimension(); i++) {
					v[i] = Danvil::clamped(s[i], _min[i], _max[i]);
				}
				return v;
			}

			State random() const {
				return RandomV(_min, _max);
			}

			State random(const State& center, const std::vector<K>& noise) {
				if(noise.size() != State::Dimension) {
					throw InvalidNoiseVectorException();
				}
				State noise_range(noise.data());
				return project(this->compose(center, RandomV(-noise_range, noise_range)));
			}

		private:
			static State RandomV(const State& min, const State& max) {
				State v;
				for(size_t i=0; i<v.dimension(); i++) {
					v[i] = RandomNumbers::S.random(min[i], max[i]);
				}
				return v;
			}

			State _min, _max;

		protected:
			~Box() {}
		};

		// TODO: allow states with variable dimension?
		template<typename State>
		struct Interval
		{
			struct InvalidNoiseVectorException {};

			Interval() {}

			Interval(const State& min, const State& max)
			: _min(min),
			  _max(max) {}

			void setDomainRange(const State& range) {
				_min = -range;
				_max = range;
			}

			void setDomainMin(const State& min) {
				_min = min;
			}

			void setDomainMax(const State& max) {
				_max = max;
			}

			size_t dimension() const {
				return 1;
			}

			State project(const State& s) const {
				return Danvil::clamped(s, _min, _max);
			}

			State random() const {
				return RandomV(_min, _max);
			}

			State random(const State& center, const std::vector<double>& noise) {
				if(noise.size() != 1) {
					throw InvalidNoiseVectorException();
				}
				State noise_range = noise[0];
				return project(this->compose(center, RandomV(-noise_range, noise_range)));
			}

		private:
			static State RandomV(const State& min, const State& max) {
				return RandomNumbers::S.random(min, max);
			}

			State _min, _max;

		protected:
			~Interval() {}
		};
	}

	//---------------------------------------------------------------------------

	template<
		typename State,
		class Operator = Operations::Linear<State>,
		class Domain = Domains::Box<State>,
		class OperationFinal = OperationFinalPolicy::Projected<State>
	>
	struct CartesianSpace
	: public BaseSpace<State, Operator, Domain, OperationFinal>
	{ };
}

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
