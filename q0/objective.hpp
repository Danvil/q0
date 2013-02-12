#ifndef Q0_OBJECTIVE_HPP_
#define Q0_OBJECTIVE_HPP_
#include <vector>
#include <boost/type_traits/function_traits.hpp>
#include <boost/function.hpp>
#include <functional>
//---------------------------------------------------------------------------
namespace q0 { namespace objective {

template<typename Objective>
struct argument_type
{
	typedef typename boost::function_traits<Objective>::arg1_type type;
};

template<typename Objective>
struct result_type
{
	typedef typename boost::function_traits<Objective>::result_type type;
};

template<typename X, typename Y>
struct argument_type<std::function<Y(const X&)>> {
	typedef X type;
};

template<typename X, typename Y>
struct argument_type<boost::function<Y(const X&)>> {
	typedef X type;
};

template<typename X, typename Y>
struct result_type<std::function<Y(const X&)>> {
	typedef Y type;
};

template<typename X, typename Y>
struct result_type<boost::function<Y(const X&)>> {
	typedef Y type;
};

template<typename Objective>
std::vector<typename result_type<Objective>::type> parallel(Objective f, const std::vector<typename argument_type<Objective>::type>& states) {
	std::vector<typename result_type<Objective>::type> scores(states.size());
	for(std::size_t i=0; i<states.size(); i++) {
		scores[i] = f(states[i]);
	}
	return scores;
}

}}
//---------------------------------------------------------------------------
#endif
