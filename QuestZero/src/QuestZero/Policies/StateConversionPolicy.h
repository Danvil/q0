/*
 * StateConversionPolicy.h
 *
 *  Created on: Sep 10, 2010
 *      Author: david
 */

#ifndef STATECONVERSIONPOLICY_H_
#define STATECONVERSIONPOLICY_H_
//---------------------------------------------------------------------------
#include "QuestZero/Exceptions.h"
//---------------------------------------------------------------------------
namespace Q0 {
namespace Spaces {
//---------------------------------------------------------------------------

namespace StateConversionPolicy
{
	template<typename State>
	struct ToArray
	{
		typedef typename State::ScalarType K;

		size_t numbersCount() const {
			return State::Dimension;
		}

		void write(size_t n, K* data) const {
			INVALID_SIZE_EXCEPTION(n != numbersCount())
			Danvil::Memops::Copy<K, State::Dimension>(data, this->begin());
		}

		void read(size_t n, const K* data) {
			INVALID_SIZE_EXCEPTION(n != numbersCount())
			Danvil::Memops::Copy<K, State::Dimension>(this->begin(), data);
		}
	};

	template<typename State>
	struct ToStdVector
	{
		typedef typename State::ScalarType K;

		size_t numbersCount() const {
			return State::Dimension;
		}

		void write(std::vector<K>& v) const {
			v.clear();
			v.reserve(numbersCount());
			Danvil::Memops::Copy<K, State::Dimension>(v.base(), this->begin());
		}

		void read(const State& state, const std::vector<K>& v) {
			INVALID_SIZE_EXCEPTION(v.size() != numbersCount())
			Danvil::Memops::Copy<K, State::Dimension>(this->begin(), v.base());
		}
	};
}

//---------------------------------------------------------------------------
}}
//---------------------------------------------------------------------------
#endif