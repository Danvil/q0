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
namespace StartingStatePicker {

	//---------------------------------------------------------------------------

	template<typename State, template <typename> class SinglePicker>
	struct ManyPicker
	: public SinglePicker<State>
	{
		template<class Space>
		std::vector<State> pickMany(const Space& space, unsigned int n) {
			std::vector<State> states;
			states.reserve(n);
			for(unsigned int i=0; i<n; i++) {
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
		State pick(const Space& space) {
			return space.random();
		}

	protected:
		~RandomPicker() {}
	};

	//---------------------------------------------------------------------------

	template<typename State>
	struct RepeatOne
	{
		void setDefaultState(const State& state) const { _defaultState = state; }

		template<class Space>
		const State& pick(PTR(Space)) {
			return _defaultState;
		}

	private:
		State _defaultState;

	protected:
		~RepeatOne() {}
	};

	//---------------------------------------------------------------------------

	template<typename State>
	struct RepeatMany
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

	protected:
		~RepeatMany() {}
	};

	//---------------------------------------------------------------------------

	template<typename State>
	struct OneWithNoise
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

	protected:
		~OneWithNoise() {}
	};

	//---------------------------------------------------------------------------

}
//---------------------------------------------------------------------------
#endif
