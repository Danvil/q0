#ifndef Q0_CONTROL_HPP_
#define Q0_CONTROL_HPP_
#include <q0/particle_vector.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <iostream>
//---------------------------------------------------------------------------
namespace q0 {

namespace control {

	template<typename State, typename Score>
	struct TestAndTrace
	{

		boost::function<bool(const State& state, const Score& score)> tester;

		boost::function<void(const particle_vector<State,Score>&, const particle<State,Score>& best)> tracer;

		bool operator()(const particle_vector<State,Score>& particles, const particle<State,Score>& best) {
			if(tracer) {
				tracer(particles, best);
			}
			if(tester) {
				return tester(best.state, best.score);
			}
			else {
				return false;
			}
		}

		bool operator()(const particle<State,Score>& best) {
			if(tracer) {
				particle_vector<State,Score> particles;
				particles.resize(1);
				particles.states[0] = best.state;
				particles.scores[0] = best.score;
				tracer(particles, best);
			}
			if(tester) {
				return tester(best.state, best.score);
			}
			else {
				return false;
			}
		}

	};

}

template<typename State, typename Score>
control::TestAndTrace<State,Score> stop_condition(bool (*tester)(State state, Score score)) {
	control::TestAndTrace<State,Score> tt;
	tt.tester = tester;
	return tt;
}

template<typename State, typename Score>
control::TestAndTrace<State,Score> stop_condition(bool (*tester)(const State& state, Score score)) {
	control::TestAndTrace<State,Score> tt;
	tt.tester = tester;
	return tt;
}

template<typename State, typename Score>
control::TestAndTrace<State,Score> stop_condition(boost::function<bool(const State& state, Score score)> tester) {
	control::TestAndTrace<State,Score> tt;
	tt.tester = tester;
	return tt;
}

}
//---------------------------------------------------------------------------
#endif
