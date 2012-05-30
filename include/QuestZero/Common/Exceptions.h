/*
 * Exceptions.h
 *
 *  Created on: Jan 7, 2011
 *      Author: david
 */

#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_
//----------------------------------------------------------------------------//
#include <stdexcept>
//----------------------------------------------------------------------------//
namespace Q0 {
//----------------------------------------------------------------------------//

namespace Exceptions
{
	class InvalidSize
	: public std::runtime_error
	{
	public:
		InvalidSize(const char* msg)
		: std::runtime_error(msg) {}
	};
}

#define INVALID_SIZE_EXCEPTION(FAIL_COND) if(FAIL_COND) { throw Exceptions::InvalidSize(#FAIL_COND); }

//----------------------------------------------------------------------------//
}
//----------------------------------------------------------------------------//
#endif
