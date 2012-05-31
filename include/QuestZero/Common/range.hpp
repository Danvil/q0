/*
 * range.hpp
 *
 *  Created on: May 31, 2012
 *      Author: david
 */

#ifndef RANGE_HPP_
#define RANGE_HPP_

#include <map> // FIXME only need pair

namespace Q0 {

namespace detail {

	template<typename T>
	struct range : public std::pair<T,T>
	{
		range() {}

		range(const std::pair<T,T>& p)
		: std::pair<T,T>(p) {}

		T begin() const { return this->first; }
		T end() const { return this->second; }
	};
}

}

#endif
