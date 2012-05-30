/*
 * TimeControl.hpp
 *
 *  Created on: Jan 23, 2012
 *      Author: david
 */

#ifndef TIMECONTROL_HPP_
#define TIMECONTROL_HPP_
//----------------------------------------------------------------------------//
#include <boost/function.hpp>
//----------------------------------------------------------------------------//
namespace Q0 {
//----------------------------------------------------------------------------//

namespace Policies
{

	struct TimeControlInfinite
	{
		bool isRunning(unsigned int i) const {
			return true;
		}
	};

	struct TimeControlDelegate
	{
		typedef boost::function<bool(unsigned int&)> TimeControlDelegateType;

		TimeControlDelegate()
		{}

		TimeControlDelegate(TimeControlDelegateType f)
		: time_control_delegate_(f) {}

		void SetTimeControlDelegate(const TimeControlDelegateType& f) {
			time_control_delegate_ = f;
		}

		bool isRunning(unsigned int i) const {
			return time_control_delegate_(i);
		}

	private:
		TimeControlDelegateType time_control_delegate_;
	};
}

//----------------------------------------------------------------------------//
}
//----------------------------------------------------------------------------//
#endif
