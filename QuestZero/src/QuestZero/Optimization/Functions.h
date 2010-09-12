#ifndef QUEST_ZERO_IFUNCTION_H
#define QUEST_ZERO_IFUNCTION_H

#include <vector>
#include <boost/function.hpp>

namespace Functions
{
	/**
	 * A valid function object should look like this:
	 * struct f {
	 *   std::vector<double> operator()(const std::vector<State>& states) { ... }
	 *   double operator()(const <State& state) { ... }
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
			for(size_t i=0; i<states.size(); ++i) {
				scores.push_back((*this)(states[i]));
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

			void setFunctor(const FunctorType& f) { _functor = f; }

			ReturnType operator()(const State& state) const {
				return _functor(state);
			}

		private:
			FunctorType _functor;
		};
	}

	template<typename State, typename ReturnType=DefaultReturnType>
	struct BoostFunctionSingleWrapper
	: public AddParallel<State, Private::BoostFunctionSingleWrapperHelper<State, ReturnType>, ReturnType>
	{};

	namespace Private
	{
		template<typename State, typename ReturnType=DefaultReturnType>
		struct BoostFunctionParallelWrapperHelper
		{
			typedef boost::function<std::vector<ReturnType>(const std::vector<State>&)> FunctorType;

			void setFunctor(const FunctorType& f) { _functor = f; }

			std::vector<ReturnType> operator()(const std::vector<State>& states) const {
				return _functor(states);
			}

		private:
			FunctorType _functor;
		};
	}

	template<typename State, typename ReturnType=DefaultReturnType>
	struct BoostFunctionParallelWrapper
	: public AddSingle<State, Private::BoostFunctionParallelWrapperHelper<State, ReturnType>, ReturnType>
	{};


}
#endif

