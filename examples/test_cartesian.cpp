#include <q0/q0.hpp>
#include <q0/algorithms.hpp>
#include <q0/domains/cartesian.hpp>
#include <q0/algorithms/random_search.hpp>
#include <q0/algorithms/apso.hpp>
#include <iostream>

unsigned int f_eval_count = 0;

float f(const Eigen::Vector2f& x) {
	f_eval_count ++;
	return x.squaredNorm();
}

bool stop_condition(const Eigen::Vector2f& u, float s) {
	return f_eval_count >= 1000 || s < 0.001f;
}

int main(int argc, char** argv)
{
	q0::domains::cartesian<float,2> dom;

	auto p = q0::minimize<q0::algorithms::apso>::apply(dom, &f, q0::stop_condition(&stop_condition));
	std::cout << "{" << p.state.x() << "," << p.state.y() << "} -> " << p.score << std::endl;
	std::cout << "Number of evaluations: " << f_eval_count << std::endl;
	return 1;
}
