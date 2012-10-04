#ifndef Q0_CONTROL_HPP_
#define Q0_CONTROL_HPP_
#include <q0/particle_vector.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
//---------------------------------------------------------------------------
namespace q0 { namespace control {

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

};

}}
//---------------------------------------------------------------------------
#endif
