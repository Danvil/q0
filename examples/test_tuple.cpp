#include <q0/q0.hpp>
#include <q0/algorithms.hpp>
#include <q0/domains/cartesian.hpp>
#include <q0/domains/so2.hpp>
#include <q0/domains/tuple.hpp>
#include <q0/algorithms/random_search.hpp>
#include <q0/algorithms/apso.hpp>
#include <tuple>
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
	float q = a*a + (2 + std::sin(b) + std::cos(10*(b)));
	return q;
}

bool check(const state_t&, float s) {
	return f_eval_count >= 1000 || s < 0.001f;
}

int main(int argc, char** argv)
{
	domain_t dom;

	std::cout << "tuple size: " << std::tuple_size<decltype(dom)>::value << std::endl;

	std::cout << "dimension: " << q0::domains::dimension(dom) << std::endl;

	state_t u = q0::domains::random(dom);
	std::cout << "random state: {" << std::get<0>(u) << "," << std::get<1>(u) << "} -> " << f(u) << std::endl;

	auto p = q0::minimize<q0::algorithms::apso>::apply(dom, &f, q0::stop_condition(&check));
	std::cout << "result: {" << std::get<0>(p.state) << "," << std::get<1>(p.state) << "} -> " << p.score << std::endl;
	std::cout << "Number of evaluations: " << f_eval_count << std::endl;
	return 1;
}
