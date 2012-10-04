#include <q0/q0.hpp>
#include <q0/algorithms.hpp>
#include <q0/domains/so2.hpp>
#include <q0/algorithms/random_search.hpp>
#include <q0/algorithms/apso.hpp>
#include <iostream>

unsigned int f_eval_count = 0;

float f(const q0::domains::angle<float>& u) {
	f_eval_count ++;
	return 2 + std::sin(u) + std::cos(10*u);
}

bool stop_condition(const q0::domains::angle<float>&, float s) {
//	std::cout << "{" << u << "} -> " << s << std::endl;
	return f_eval_count >= 1000 || s < 0.001f;
}

int main(int argc, char** argv)
{
	q0::domains::so2<float> dom;

	auto p = q0::minimize<q0::algorithms::apso>::apply(dom, &f, &stop_condition);
	std::cout << "{" << p.state << "} -> " << p.score << std::endl;
	std::cout << "Number of evaluations: " << f_eval_count << std::endl;
	return 1;
}
