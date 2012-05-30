/*
 * GetScalarType.h
 *
 *  Created on: Sep 13, 2010
 *      Author: david
 */

#ifndef GETSCALARTYPE_H_
#define GETSCALARTYPE_H_
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

namespace Private
{
	template<typename X>
	struct GetScalarType { typedef typename X::ScalarType ScalarType; };

	template<>
	struct GetScalarType<float> { typedef float ScalarType; };

	template<>
	struct GetScalarType<double> { typedef double ScalarType; };
};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
