#include <q0/q0.hpp>
#include <q0/algorithms.hpp>
#include <q0/domains/cartesian.hpp>
#include <q0/algorithms/apso.hpp>
#include <q0/control.hpp>
#include <iostream>

typedef q0::domains::cartesian<float,2,q0::domains::cartesian_constraint_box> domain_t;
typedef typename q0::domains::state_type<domain_t>::type state_t;
typedef float score_t;

unsigned int f_eval_count = 0;

score_t f(const state_t& x) {
	f_eval_count ++;
	return x.squaredNorm();
}

bool stop_condition(const state_t& u, score_t s) {
	return f_eval_count >= 1000 || s < 0.001f;
}

void tracer(const q0::particle_vector<state_t,score_t>& particles, const q0::particle<state_t,score_t>& best) {
	static int i = 0;
	std::cout << "Iteration " << i << ": " << particles.size() << " particles, best = {" << best.state.x() << "," << best.state.y() << "} -> " << best.score << std::endl;
	i++;
}

int main(int argc, char** argv)
{
	domain_t dom;
	dom.set_box_extends(-1,+2);

	q0::algorithms::apso alg;

	q0::control::TestAndTrace<state_t, score_t> control;
	control.tester = &stop_condition;
	control.tracer = &tracer;

	auto p = q0::minimize(dom, f, alg, control);

	std::cout << "{" << p.state.x() << "," << p.state.y() << "} -> " << p.score << std::endl;
	std::cout << "Number of evaluations: " << f_eval_count << std::endl;
	
	return 1;
}
