#include <q0/q0.hpp>
#include <q0/algorithms.hpp>
#include <q0/domains/cartesian.hpp>
#include <q0/domains/so2.hpp>
#include <q0/domains/vector.hpp>
#include <q0/algorithms/random_search.hpp>
#include <q0/algorithms/apso.hpp>
#include <tuple>
#include <iostream>

typedef q0::domains::vector<q0::domains::so2<float>> domain_t;

typedef q0::domains::state_type<domain_t>::type state_t;

unsigned int f_eval_count = 0;

float f(const state_t& u) {
	f_eval_count ++;
	float a = u[0];
	float b = u[1];
	float q = (2 + std::sin(a) + std::cos(10*(a)))
			+ (2 + std::sin(b+1) + std::cos(10*(b+1)));
	return q;
}

bool check(const state_t&, float s) {
	return f_eval_count >= 1000 || s < 0.001f;
}

int main(int argc, char** argv)
{
	domain_t dom(2);

	std::cout << "tangent size: " << q0::domains::tangent_size<domain_t>::value << std::endl;

	std::cout << "dimension: " << q0::domains::dimension(dom) << std::endl;

	state_t u = q0::domains::random(dom);
	std::cout << "random state: {" << u[0] << "," << u[1] << "} -> " << f(u) << std::endl;

	auto p = q0::minimize<q0::algorithms::apso>::apply(dom, &f, q0::stop_condition(&check));
	std::cout << "result: {" << p.state[0] << "," << p.state[1] << "} -> " << p.score << std::endl;
	std::cout << "Number of evaluations: " << f_eval_count << std::endl;
	return 1;
}
