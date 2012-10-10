#include <q0/q0.hpp>
#include <q0/algorithms.hpp>
#include <q0/domains/cartesian.hpp>
#include <q0/algorithms/random_search.hpp>
#include <q0/algorithms/apso.hpp>
#include <q0/algorithms/differential_evolution.hpp>
#include "SpecialFunctions.hpp"
#include <boost/shared_ptr.hpp>
#include <iostream>

template<typename K, int N>
struct Problem
{
	typedef q0::domains::cartesian<K,N> domain_t;
	typedef typename q0::domains::state_type<domain_t>::type state_t;

	Problem() {
		eval_count_.reset(new unsigned int());
		*eval_count_ = 0;
	}

	K operator()(const state_t& x) const {
		*eval_count_ = *eval_count_ + 1;
		return f_(x);
	}

	bool stop_condition_count(const state_t&, K s) {
		return *eval_count_ >= 500;
	}

	bool stop_condition_score(const state_t&, K s) {
		return *eval_count_ >= 10000 || s < 0.01f;
	}

	domain_t dom_;

	boost::function<K(const Eigen::Matrix<K,N,1>&)> f_;

	boost::shared_ptr<unsigned int> eval_count_;

};

namespace q0 { namespace objective {

template<typename K, int N>
struct argument_type<Problem<K,N>> { typedef typename Problem<K,N>::state_t type; };

template<typename K, int N>
struct result_type<Problem<K,N>> { typedef K type; };

}}

template<typename K, int N>
Problem<K,N> BuildProblem(boost::function<K(const Eigen::Matrix<K,N,1>&)> f) {
	Problem<K,N> x;
	x.f_ = f;
	return x;
}

template<typename K, int N, template<class,class,class,class>class Algo>
void run(const std::string& name_algo, const std::string& name_f, boost::function<K(const Eigen::Matrix<K,N,1>&)> f)
{
	typedef Problem<K,N> problem_t;

	std::cout << name_f << "\t" << name_algo << std::flush;

	float sum_score = 0.0f;
	unsigned int sum_count = 0;
	unsigned int sum_count_failures = 0;
	constexpr unsigned int reps = 100;

	for(unsigned int i=0; i<reps; i++) {
		problem_t problem = BuildProblem(f);
		boost::function<bool(const typename problem_t::state_t&,K)> tester = boost::bind(&problem_t::stop_condition_score, problem, _1, _2);
		q0::minimize<Algo>::apply(problem.dom_, problem, q0::stop_condition(tester));
		if(*problem.eval_count_ >= 10000) {
			sum_count_failures ++;
		}
		else {
			sum_count += *problem.eval_count_;
		}
	}

	for(unsigned int i=0; i<reps; i++) {
		problem_t problem = BuildProblem(f);
		boost::function<bool(const typename problem_t::state_t&,K)> tester = boost::bind(&problem_t::stop_condition_count, problem, _1, _2);
		auto p = q0::minimize<Algo>::apply(problem.dom_, problem, q0::stop_condition(tester));
		sum_score += p.score;
	}

	std::cout
		<< "\t" << sum_score/static_cast<float>(reps)
		<< "\t" << static_cast<float>(sum_count)/static_cast<float>(reps - sum_count_failures)
		<< "\t" << sum_count_failures
		<< std::endl;
}

#define RUN(N,X,F) run<float,N,q0::algorithms::X>(#X,#F,boost::bind(&q0::SpecialFunctions<float,N>::F,_1));

#define RUN_A(N,F) \
	RUN(N,monte_carlo,F) \
	RUN(N,local_unimodal_search,F) \
	RUN(N,pattern_search,F) \
	RUN(N,simulated_annealing,F) \
	RUN(N,apso,F) \
	RUN(N,differential_evolution,F)

#define RUN_A_F(N) \
	RUN_A(N,Sphere) \
	RUN_A(N,Schwefel2_22) \
	RUN_A(N,Rosenbrock) \
	RUN_A(N,Himmelblau) \
	RUN_A(N,Rastrigin) \
	RUN_A(N,Ackley)

int main(int argc, char** argv)
{
	unsigned int seed = 0;
	seed = static_cast<unsigned int>(std::time(0));
	q0::math::random_seed(seed);
	std::cout << "Seed = " << seed << std::endl;

	RUN_A_F(2)

	return 1;
}
