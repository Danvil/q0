#ifndef QUEST_ZERO_ITRACER_H_
#define QUEST_ZERO_ITRACER_H_
//---------------------------------------------------------------------------
#include "QuestZero/Common/SampleSet.h"
#include "QuestZero/Tracking/experimental/Solution.h"
#include <boost/function.hpp>
#include <iostream>
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

/** Policies for solution notification */
namespace Policies
{
	/** Does nothing */
	template<typename State, typename Score>
	struct TimestepResultDummy
	{
		typedef TSample<State, Score> SampleType;

		void NotifyTimestepResult(unsigned int, const SampleType&) {}
	};

	/** Prints the current best sample to a stream */
	template<typename State, typename Score>
	struct TimestepResultCout
	{
		typedef TSample<State, Score> SampleType;

		void NotifyTimestepResult(unsigned int time, const SampleType& sample) {
			std::cout << "Current timestep: time=" << time << ", sample=" << sample << std::endl;
		}
	};

	/** Forwards the current best notification to a delegate */
	template<typename State, typename Score>
	struct TimestepResultDelegate
	{
		typedef TSample<State, Score> SampleType;

		typedef boost::function<void(unsigned int, const SampleType&)> TimestepResultDelegateType;

		void SetTimestepResultDelegate(const TimestepResultDelegateType& f) {
			timestep_result_delegate_ = f;
		}

		void NotifyTimestepResult(unsigned int time, const SampleType& sample) {
			if(timestep_result_delegate_) {
				timestep_result_delegate_(time, sample);
			}
		}

	private:
		TimestepResultDelegateType timestep_result_delegate_;
	};
}

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
