/*
 * InitialStatesPolicy.h
 *
 *  Created on: Sep 7, 2010
 *      Author: david
 */

#ifndef QUESTZERO_INITIALSTATESPOLICY_H
#define QUESTZERO_INITIALSTATESPOLICY_H
//---------------------------------------------------------------------------
#include <Danvil/Ptr.h>
#include <vector>
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------
namespace InitialStatesPolicy {

	//---------------------------------------------------------------------------

	template<typename State, template <typename> class SinglePicker>
	struct ManyPicker
	: public SinglePicker<State>
	{
		template<class Space>
		std::vector<State> pickMany(const Space& space, size_t n) {
			std::vector<State> states;
			states.reserve(n);
			for(size_t i=0; i<n; ++i) {
				states.push_back(this->pick(space));
			}
			return states;
		}

	protected:
		~ManyPicker() {}
	};

	//---------------------------------------------------------------------------

	template<typename State>
	struct RandomPicker
	{
		template<class Space>
		State pick(const Space& space) const {
			return space.random();
		}

	protected:
		~RandomPicker() {}
	};

	//---------------------------------------------------------------------------

	template<typename State>
	struct RepeatOne
	{
		void setDefaultState(const State& state) {
			LOG_DEBUG << "RepeatOne default state: " << state;
			default_state_ = state;
		}

		template<class Space>
		const State& pick(const Space&) const {
			return default_state_;
		}

	private:
		State default_state_;

	protected:
		~RepeatOne() {}
	};

	//---------------------------------------------------------------------------

	template<typename State>
	struct RepeatMany
	{
		RepeatMany() : index_(0) {}

		struct EmptyValueListException : public std::runtime_error {};

		void setValues(const std::vector<State>& states) {
			states_ = states;
		}

		template<class Space>
		const State& pick(const Space&) {
			if(states_.size() == 0) {
				throw EmptyValueListException();
			}
			const State& state = states_[index_];
			index_ = (index_ + 1) % states_.size();
			return state;
		}

	private:
		size_t index_;
		std::vector<State> states_;

	protected:
		~RepeatMany() {}
	};

	//---------------------------------------------------------------------------

	template<typename State>
	struct OneWithNoise
	{
		void setMeanStartingValue(const State& state) {
			_state = state;
		}

		void setStartingNoise(const std::vector<double>& noise) {
			_noise = noise;
		}

		template<class Space>
		State pick(const Space& space) const{
			return space.random(_state, _noise);
		}

	private:
		State _state;
		std::vector<double> _noise;

	protected:
		~OneWithNoise() {}
	};

	//---------------------------------------------------------------------------

}
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
