#ifndef Q0_Q0_HPP_
#define Q0_Q0_HPP_
#include <q0/control.hpp>
#include <boost/function.hpp>
//---------------------------------------------------------------------------
namespace q0 {

// template<typename State, typename Score>
// TestAndTrace<State,Score> create_stop_condition(bool(f*)(const State& state, Score s)) {
// 	TestAndTrace<State,Score> tt;
// 	tt.tester = boost::bind(f, _1, _2);
// 	return tt;
// }

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
