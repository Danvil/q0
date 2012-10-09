#include <q0/q0.hpp>
#include <q0/algorithms.hpp>
#include <q0/domains/cartesian.hpp>
#include <q0/algorithms/random_search.hpp>
#include <q0/algorithms/apso.hpp>
#include <q0/algorithms/differential_evolution.hpp>
#include <iostream>

typedef q0::domains::cartesian<float,2> domain_t;
typedef typename q0::domains::state_type<domain_t>::type state_t;

unsigned int f_eval_count = 0;

unsigned int seed = 0;

float f(const state_t& x) {
	f_eval_count ++;
	return x.squaredNorm();
}

bool stop_condition(const state_t& u, float s) {
//	std::cout << "{" << u.x() << "," << u.y() << "} -> " << s << std::endl;
	return f_eval_count >= 1000 || s < 0.005f;
}

template<template<class,class,class,class>class Algo>
void run(const std::string& name)
{
	q0::math::random_seed(seed);
	f_eval_count = 0;
	std::cout << "" << std::endl;
	std::cout << ">>> " << name << std::endl;
	domain_t dom;
	auto p = q0::minimize<Algo>::apply(dom, &f, q0::stop_condition(&stop_condition));
	std::cout << "{" << p.state.x() << "," << p.state.y() << "} -> " << p.score << std::endl;
	std::cout << "Number of evaluations: " << f_eval_count << std::endl;
}

#define RUN(X) run<X>(#X)

int main(int argc, char** argv)
{
	seed = static_cast<unsigned int>(std::time(0));
	std::cout << "Seed = " << seed << std::endl;

	RUN(q0::algorithms::monte_carlo_1);
	RUN(q0::algorithms::monte_carlo);
	RUN(q0::algorithms::random_walk_1);
	RUN(q0::algorithms::random_walk);
	RUN(q0::algorithms::local_unimodal_search_1);
	RUN(q0::algorithms::local_unimodal_search);
	RUN(q0::algorithms::pattern_search_1);
	RUN(q0::algorithms::pattern_search);
	RUN(q0::algorithms::apso);
	RUN(q0::algorithms::differential_evolution);
	return 1;
}
