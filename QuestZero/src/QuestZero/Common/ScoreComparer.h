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

template<typename Score>
struct BetterMeansSmaller
{
	static bool compare(Score a, Score b) {
		return a < b;
	}
	bool operator()(Score a, Score b) {
		return compare(a, b);
	}
	template<typename U>
	bool operator()(const TSample<U,Score>& a, const TSample<U,Score>& b) {
		return compare(a.score(), b.score());
	}
};

template<typename Score>
struct BetterMeansBigger
{
	static bool compare(Score a, Score b) {
		return a > b;
	}
	bool operator()(Score a, Score b) {
		return compare(a, b);
	}
	template<typename U>
	bool operator()(const TSample<U,Score>& a, const TSample<U,Score>& b) {
		return compare(a, b);
	}
};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
