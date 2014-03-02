#include <q0/q0.hpp>
#include <q0/algorithms.hpp>
#include <q0/domains/cartesian.hpp>
#include <q0/domains/so2.hpp>
#include <q0/domains/tuple.hpp>
#include <q0/algorithms/apso.hpp>
#include <iostream>

typedef q0::domains::tuple<
	q0::domains::cartesian<float,1>,
	q0::domains::so2<float>
> domain_t;

typedef q0::domains::state_type<domain_t>::type state_t;

unsigned int f_eval_count = 0;

float f(const state_t& u) {
	f_eval_count ++;
	float a = std::get<0>(u);
	q0::domains::angle<float> b = std::get<1>(u);
	float q = a*a + (2.0f + std::sin(b) + std::cos(10.0f*b));
	return q;
}

bool check(const state_t&, float s) {
	return f_eval_count >= 1000 || s < 0.001f;
}

int main(int argc, char** argv)
{
	domain_t dom;

	std::cout << "Domain dimension: " << q0::domains::dimension(dom) << std::endl;
	std::cout << "Domain tangent size: " << q0::domains::tangent_size<domain_t>::value << std::endl;

	state_t u = q0::domains::random(dom);
	std::cout << "Random: " << q0::print(dom,u) << " -> " << f(u) << std::endl;

	state_t expected = state_t(0,4.7123889);
	std::cout << "Expected: " << q0::print(dom,expected) << " -> " << f(expected) << std::endl;

	q0::algorithms::apso alg;
	auto p = q0::minimize(dom, f, alg, q0::stop_condition(&check));
	std::cout << "Actual:   " << q0::print(dom,p.state) << " -> " << p.score << std::endl;
	std::cout << "Number of evaluations: " << f_eval_count << std::endl;
	return 1;
}
