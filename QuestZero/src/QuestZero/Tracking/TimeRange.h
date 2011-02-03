/*
 * TimeRange.h
 *
 *  Created on: Sep 12, 2010
 *      Author: david
 */

#ifndef TIMERANGE_H_
#define TIMERANGE_H_
//---------------------------------------------------------------------------
#include <Danvil/Tools/MoreMath.h>
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

template<typename Time_>
struct TTimeRange
{
	// TODO RangeType is expected to be a integer type

	typedef Time_ Time;

	struct OutOfRangeException {};

	TTimeRange()
	: begin_(0),
	  end_(0),
	  fps_(60.0)
	{}

	TTimeRange(Time a, Time b, double fps=60.0)
	: begin_(a),
	  end_(b),
	  fps_(fps)
	{}

	/** First time */
	const Time& begin() const { return begin_; }

	/** Last time (inclusive) */
	const Time& last() const { return end_ - 1; }

	/** Time after last time (exclusive) */
	const Time& end() const { return end_; }

	bool isEmpty() const {
		return end_ == begin_;
	}

	Time frameCount() const {
		return end_ - begin_;
	}

	double framerate() const { return fps_; }

	void setBegin(const Time& t) { begin_ = t; }

	void setEnd(const Time& t) { end_ = t; }

	void setFramerate(double fps) { fps_ = fps; }

	bool isValidTime(const Time& t) const {
		return begin_ <= t  && t < end_;
	}

	size_t timeToIndex(const Time& t) const{
		if(!isValidTime(t)) {
			throw OutOfRangeException();
		}
		return (size_t)(t - begin_);
	}

	Time indexToTime(size_t frame) const {
		if(frame < 0 || (size_t)frameCount() <= frame) {
			throw OutOfRangeException();
		}
		return (Time)(begin_ + frame);
	}

	size_t percentToIndex(float p) const {
		if(p < 0 || 1 < p) {
			throw OutOfRangeException();
		}
		return Danvil::MoreMath::RoundToInt(float(begin_) + p * float(end_ - begin_ - 1));
	}

	/** Number in [0,1[ indicating the position of the given time in the time range */
	double percent(const Time& t) const {
		return (double)timeToIndex(t) / (double)(end_ - 1 - begin_);
	}

private:
	Time begin_;
	Time end_;
	double fps_;
};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
