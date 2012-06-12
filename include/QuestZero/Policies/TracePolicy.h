#ifndef QUEST_ZERO_ITRACER_H_
#define QUEST_ZERO_ITRACER_H_
//---------------------------------------------------------------------------
#include "QuestZero/Common/SampleSet.h"
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

		typedef boost::function<void(const Sample<State, Score>&)> Functor;

		void SetNotifySamplesFunctor(const Functor& f) {
			samples_functor_ = f;
		}

		template<typename SampleSet>
		void NotifySamples(const SampleSet& samples) {
			if(samples_functor_) {
				samples_functor_(get_sample(samples, find_best_by_score(samples, compare)));
			}
		}

	private:
		Functor samples_functor_;
	};
}

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
