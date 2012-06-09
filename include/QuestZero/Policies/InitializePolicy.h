/*
 * InitialStatesPolicy.h
 *
 *  Created on: Sep 7, 2010
 *      Author: david
 */

#ifndef QUESTZERO_INITIALSTATESPOLICY_H
#define QUESTZERO_INITIALSTATESPOLICY_H
//---------------------------------------------------------------------------
#include <vector>
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

namespace InitializePolicy
{

	/** Picks a random state each time using the random function of the space */
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

	/** Always picks the previously set state without modification */
	template<typename State>
	struct RepeatOne
	{
		void setDefaultState(const State& state) {
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

	/** Repeatedly picks states from a previously set of states without modification */
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

	/** Picks a randomly modified state by using the random function of the space */
	template<typename State>
	struct OneWithNoise
	{
		void set_mean(const State& state) {
			mean_ = state;
		}

		void set_noise(const std::vector<double>& noise) {
			noise_ = noise;
		}

		template<class Space>
		State pick(const Space& space) const {
			return space.random(mean_, noise_);
		}

		template<class Space>
		std::vector<State> pickMany(const Space& space, size_t n) {
			std::vector<State> states;
			states.reserve(n);
			if(n == 0) {
				return states;
			}
			states.push_back(mean_);
			for(size_t i=0; i<n - 1; i++) {
				states.push_back(this->pick(space));
			}
			return states;
		}

	private:
		State mean_;
		std::vector<double> noise_;

	protected:
		~OneWithNoise() {}
	};

}

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
