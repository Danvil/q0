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

template<typename State>
struct BetterMeansSmaller
{
	bool operator()(double a, double b) {
		return a < b;
	}
	bool operator()(const TSample<State>& a, const TSample<State>& b) {
		return a.score() < b.score();
	}
};

template<typename State>
struct BetterMeansBigger
{
	bool operator()(double a, double b) {
		return a > b;
	}
	bool operator()(const TSample<State>& a, const TSample<State>& b) {
		return a.score() > b.score();
	}
};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
