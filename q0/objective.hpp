#ifndef Q0_OBJECTIVE_HPP_
#define Q0_OBJECTIVE_HPP_
#include <vector>
//---------------------------------------------------------------------------
namespace q0 { namespace objective {

template<typename Objective>
struct argument_type;

template<typename Objective>
struct result_type;

template<typename X, typename Y>
struct argument_type<Y (*)(X)> {
	typedef X type;
};

template<typename X, typename Y>
struct argument_type<Y (*)(X&)> {
	typedef X type;
};

template<typename X, typename Y>
struct argument_type<Y (*)(const X&)> {
	typedef X type;
};

template<typename X, typename Y>
struct result_type<Y (*)(X)> {
	typedef Y type;
};

template<typename Objective>
std::vector<typename result_type<Objective>::type> parallel(const Objective& f, const std::vector<typename argument_type<Objective>::type>& states) {
	std::vector<typename result_type<Objective>::type> scores(states.size());
	for(std::size_t i=0; i<states.size(); i++) {
		scores[i] = f(states[i]);
	}
	return scores;
}

}}
//---------------------------------------------------------------------------
#endif
