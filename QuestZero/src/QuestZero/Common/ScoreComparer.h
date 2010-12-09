/*
 * ScoreComparer.h
 *
 *  Created on: Sep 13, 2010
 *      Author: david
 */

#ifndef COMMON_SCORECOMPARER_H_
#define COMMON_SCORECOMPARER_H_
//---------------------------------------------------------------------------
#include "Sample.h"
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

template<typename State, typename Score>
struct BetterMeansSmaller
{
	bool operator()(Score a, Score b) {
		return a < b;
	}
	bool operator()(const TSample<State,Score>& a, const TSample<State,Score>& b) {
		return a.score() < b.score();
	}
};

template<typename State, typename Score>
struct BetterMeansBigger
{
	bool operator()(Score a, Score b) {
		return a > b;
	}
	bool operator()(const TSample<State,Score>& a, const TSample<State,Score>& b) {
		return a.score() > b.score();
	}
};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
