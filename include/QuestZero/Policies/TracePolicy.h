#ifndef QUEST_ZERO_ITRACER_H_
#define QUEST_ZERO_ITRACER_H_
//---------------------------------------------------------------------------
#include "QuestZero/Common/SampleSet.h"
#include "QuestZero/Tracking/Solution.h"
#include <boost/function.hpp>
#include <iostream>
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

namespace TracePolicy
{
	/** Does nothing */
	struct None
	{
		template<typename SampleSet>
		void NotifySamples(const SampleSet&) {}
	};

	/** Prints the best particle to the console */
	template<typename Compare>
	struct BestToConsole
	{
		Compare compare;

		template<typename SampleSet>
		void NotifySamples(const SampleSet& samples) {
			std::cout << "Current best sample: " << get_sample(samples, find_best_by_score(samples, compare)) << std::endl;
		}
	};

	/** Prints all particles to the console */
	struct AllToConsole
	{
		template<typename SampleSet>
		void NotifySamples(const SampleSet& samples) {
			samples.printInLines(std::cout);
		}
	};

	/** Forward best particle to a functor */
	template<typename State, typename Score, typename Compare>
	struct BestToFunctorImpl
	{
		Compare compare;

		typedef boost::function<void(const State&, Score)> Functor;

		void SetNotifySamplesFunctor(const Functor& f) {
			samples_functor_ = f;
		}

		template<typename SampleSet>
		void NotifySamples(const SampleSet& samples) {
			if(samples_functor_) {
				TSample<State,Score> best = get_sample(samples, find_best_by_score(samples, compare));
				samples_functor_(best.state(), best.score());
			}
		}

	private:
		Functor samples_functor_;
	};
}

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
