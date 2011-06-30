#ifndef QUEST_ZERO_ITRACER_H_
#define QUEST_ZERO_ITRACER_H_
//---------------------------------------------------------------------------
#include "QuestZero/Common/SampleSet.h"
#include "QuestZero/Common/ScoreComparer.h"
#include "QuestZero/Tracking/Solution.h"
#include <boost/function.hpp>
#include <iostream>
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

namespace TracePolicy
{
	/** Policies for samples notification */
	namespace Samples
	{
		/** Does nothing */
		template<typename State, typename Score>
		struct None {
			void NotifySamples(const TSampleSet<State,Score>&) {}
		};
		/** Prints the best particle to the console */
		template<typename State, typename Score, template<typename> class ScoreCmp=BetterMeansSmaller>
		struct BestToConsole {
			void NotifySamples(const TSampleSet<State,Score>& samples) {
				std::cout << "Current best sample: " << samples.template FindBestSample<ScoreCmp<Score> >() << std::endl;
			}
		};
		/** Prints all particles to the console */
		template<typename State, typename Score>
		struct AllToConsole {
			void NotifySamples(const TSampleSet<State,Score>& samples) {
				samples.printInLines(std::cout);
			}
		};
		/** Prints particle with smallest score to the console */
		template<typename State, typename Score>
		struct SmallestToConsole : BestToConsole<State, Score, BetterMeansSmaller> {};
		/** Forward best particle to functor */
		template<typename State, typename Score, template<typename> class ScoreCmp>
		struct BestToFunctorImpl {
			typedef boost::function<void(const State&, Score score)> Functor;
			void SetNotifySamplesFunctor(const Functor& f) {
				samples_functor_ = f;
			}
			void NotifySamples(const TSampleSet<State,Score>& samples) {
				if(samples_functor_) {
					TSample<State,Score> best = samples.template FindBestSample<ScoreCmp<Score> >();
					samples_functor_(best.state(), best.score());
				}
			}
		private:
			Functor samples_functor_;
		};
		/** Forward particle with smallest score to functor */
		template<typename State, typename Score, bool DoMinimize> struct BestToFunctor;
		template<typename State, typename Score> struct BestToFunctor<State,Score,true> : public BestToFunctorImpl<State, Score, BetterMeansSmaller> {};
		template<typename State, typename Score> struct BestToFunctor<State,Score,false> : public BestToFunctorImpl<State, Score, BetterMeansBigger> {};
		/** Forwards the call to a function */
		template<typename State, typename Score>
		struct ForwardToFunction {
			typedef boost::function<void(const TSampleSet<State,Score>&)> Functor;
			void SetNotifySamplesFunctor(const Functor& f) {
				samples_functor_ = f;
			}
			void NotifySamples(const TSampleSet<State,Score>& samples) {
				if(samples_functor_) {
					samples_functor_(samples);
				}
			}
		private:
			Functor samples_functor_;
		};
		/** Forwards the call to a object */
		template<typename State, typename Score, typename X>
		struct ForwardToObject {
			void SetNotifySamplesObject(X* f) {
				notify_samples_forward_ = f;
			}
			void NotifySamples(const TSampleSet<State,Score>& samples) {
				notify_samples_forward_->NotifySamples(samples);
			}
		private:
			X* notify_samples_forward_;
		};

	}

	/** Policies for solution notification */
	namespace Solution
	{
		/** Does nothing */
		template<typename Time, typename State, typename Score>
		struct None {
			void NotifySolution(const TSolution<Time, State, Score>&) {}
		};
		/** Prints the solution to the console */
		template<typename Time, typename State, typename Score>
		struct Console {
			void NotifySolution(const TSolution<Time, State, Score>& solution) {
				std::cout << "Current solution: " << solution << std::endl;
			}
		};
		/** Forwards the call to a function */
		template<typename Time, typename State, typename Score>
		struct Forward {
			typedef boost::function<void(const TSolution<Time, State, Score>&)> Functor;
			void SetNotifySolutionFunctor(const Functor& f) {
				solution_functor_ = f;
			}
			void NotifySolution(const TSolution<Time, State, Score>& solution) {
				if(solution_functor_) {
					solution_functor_(solution);
				}
			}
		private:
			Functor solution_functor_;
		};
	}
}

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
