/*
 * Sample.h
 *
 *  Created on: May 31, 2012
 *      Author: david
 */

#ifndef SAMPLE_H_
#define SAMPLE_H_
//---------------------------------------------------------------------------
#include <iostream>
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

template<typename State, typename Score>
struct TSample
{
	State state;
	Score score;
};

template<typename State, typename Score>
std::ostream& operator<<(std::ostream& os, const TSample<State,Score>& sample) {
	os << "[Sample: state=" << sample.state << ", score=" << sample.score << "]";
	return os;
}

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------

#endif
