/*
 * StartingStatesPolicy.h
 *
 *  Created on: Sep 7, 2010
 *      Author: david
 */

#ifndef QUESTZERO_STARTINGSTATESPOLICY_H_
#define QUESTZERO_STARTINGSTATESPOLICY_H_
//---------------------------------------------------------------------------
#include <Danvil/Ptr.h>
#include <vector>
//---------------------------------------------------------------------------
namespace StatePicker {

	//---------------------------------------------------------------------------

	template<typename State>
	struct ManyPicker
	{
		template<class Space>
		std::vector<State> pick(const Space& space, unsigned int n) {
			std::vector<State> states;
			states.reserve(n);
			for(unsigned int i=0; i<n; i++) {
				states.push_back(pick(space));
			}
			return states;
		}
	};

	//---------------------------------------------------------------------------

	template<typename State>
	struct RandomPicker
	: public ManyPicker<State>
	{
		template<class Space>
		State pick(const Space& space) {
			return space.random();
		}
	};

	//---------------------------------------------------------------------------

	template<typename State>
	struct RepeatOne
	: public ManyPicker<State>
	{
		void setDefaultState(const State& state) const { _defaultState = state; }

		template<class Space>
		const State& pick(PTR(Space)) {
			return _defaultState;
		}

	private:
		State _defaultState;
	};

	//---------------------------------------------------------------------------

	template<typename State>
	struct RepeatMany
	: public ManyPicker<State>
	{
		RepeatMany() : _index(0) {}

		struct EmptyValueListException {};

		void setValues(const std::vector<State>& states) {
			_states = states;
		}

		template<class Space>
		const State& pick(PTR(Space)) {
			if(_states.size() == 0) {
				throw EmptyValueListException();
			}
			const State& state = _states[_index];
			_index = (_index + 1) % _states.size();
			return state;
		}

	private:
		size_t _index;
		std::vector<State> _states;
	};

	//---------------------------------------------------------------------------

	template<typename State>
	struct OneWithNoise
	: public ManyPicker<State>
	{
		void setValue(const State& state) {
			_state = state;
		}

		void setNoise(const std::vector<typename State::ScalarType>& noise) {
			_noise = noise;
		}

		template<class Space>
		State pick(const Space& space) {
			return space.random(_state, _noise);
		}

	private:
		State _state;
		std::vector<typename State::ScalarType> _noise;
	};

	//---------------------------------------------------------------------------

}
//---------------------------------------------------------------------------
#endif
