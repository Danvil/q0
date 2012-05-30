#ifndef QUEST_ZERO_IFUNCTION_H
#define QUEST_ZERO_IFUNCTION_H
//---------------------------------------------------------------------------
#include <vector>
#include <boost/function.hpp>
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

namespace Functions
{
	/**
	 * A valid function object should look like this:
	 * struct f {
	 *   std::vector<double> operator()(const std::vector<State>& states) { ... }
	 *   double operator()(const State& state) { ... }
	 * };
	 * If only one of the two methods is present, you can use the base classes
	 *  - AddParallel
	 *  - AddSingle
	 * to add the missing functionality in a canonical way.
	 */

	typedef double DefaultReturnType;

	template<typename State, class SingleFunction, typename ReturnType=DefaultReturnType>
	struct AddParallel
	: public SingleFunction
	{
		using SingleFunction::operator();

		std::vector<ReturnType> operator()(const std::vector<State>& states) const {
			std::vector<ReturnType> scores;
			scores.reserve(states.size());
			for(typename std::vector<State>::const_iterator it=states.begin(); it!=states.end(); ++it) {
				scores.push_back((*this)(*it));
			}
			return scores;
		}
	};

	template<typename State, class ParallelFunction, typename ReturnType=DefaultReturnType>
	struct AddSingle
	: public ParallelFunction
	{
		using ParallelFunction::operator();

		ReturnType operator()(const State& state) const {
			return operator()(std::vector<State>(1, state))[0];
		}
	};

	namespace Private
	{
		template<typename State, typename ReturnType=DefaultReturnType>
		struct BoostFunctionSingleWrapperHelper
		{
			typedef boost::function<ReturnType(const State&)> FunctorType;

			void set_functor(const FunctorType& f) { functor_ = f; }

			ReturnType operator()(const State& state) const {
				return functor_(state);
			}

		private:
			FunctorType functor_;
		};
	}

	/**
	 * An evaluation function object constructed from a boost::function which evaluates a single state
	 */
	template<typename State, typename ReturnType=DefaultReturnType>
	struct BoostFunctionSingleWrapper
	: public AddParallel<State, Private::BoostFunctionSingleWrapperHelper<State, ReturnType>, ReturnType>
	{
		typedef ReturnType Score;
	};

	namespace Private
	{
		template<typename State, typename ReturnType=DefaultReturnType>
		struct BoostFunctionParallelWrapperHelper
		{
			typedef boost::function<std::vector<ReturnType>(const std::vector<State>&)> FunctorType;

			void set_functor(const FunctorType& f) { functor_ = f; }

			std::vector<ReturnType> operator()(const std::vector<State>& states) const {
				return functor_(states);
			}

		private:
			FunctorType functor_;
		};
	}

	/**
	 * An evaluation function object constructed from a boost::function which evaluates a list of states
	 */
	template<typename State, typename ReturnType=DefaultReturnType>
	struct BoostFunctionParallelWrapper
	: public AddSingle<State, Private::BoostFunctionParallelWrapperHelper<State, ReturnType>, ReturnType>
	{
		typedef ReturnType Score;
	};


}

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif

