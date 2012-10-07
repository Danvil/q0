#include <q0/q0.hpp>
#include <q0/domains/cartesian.hpp>
#include <q0/domains/so2.hpp>
#include <q0/domains/vector.hpp>
#include <q0/domains/tuple.hpp>
#include <q0/print.hpp>
#include <iostream>

template<typename Domain>
void run(const Domain& dom, const std::string& name)
{
	std::cout << std::endl;
	std::cout << ">>> " << name << std::endl;
	typedef typename q0::domains::state_type<Domain>::type state_t;

	state_t x = q0::domains::random(dom);
	std::cout << q0::print(dom,x) << std::endl;
}

int main(int argc, char** argv)
{
	{
		typedef q0::domains::cartesian<float,3> domain_t;
		domain_t dom;
		run<domain_t>(dom, "q0::domains::cartesian<float,3>");
	}

	{
		typedef q0::domains::cartesian<float,0> domain_t;
		domain_t dom;
		run<domain_t>(dom, "q0::domains::cartesian<float,0>");
	}

	{
		typedef q0::domains::vector<q0::domains::so2<float>> domain_t;
		domain_t dom(3);
		run<domain_t>(dom, "q0::domains::vector<q0::domains::so2<float>> (size=3)");
	}

	{
		typedef q0::domains::vector<q0::domains::so2<float>> domain_t;
		domain_t dom(0); // size = 0
		run<domain_t>(dom, "q0::domains::vector<q0::domains::so2<float>> (size=0)");
	}

	{
		typedef q0::domains::tuple< q0::domains::cartesian<float,3> , q0::domains::so2<float> > domain_t;
		domain_t dom;
		run<domain_t>(dom, "q0::domains::tuple< q0::domains::cartesian<float,3> , q0::domains::so2<float> >");
	}

	{
		typedef q0::domains::tuple< q0::domains::cartesian<float,3> , q0::domains::vector<q0::domains::so2<float>> > domain_t;
		domain_t dom;
		std::get<1>(dom).resize(3);
		run<domain_t>(dom, "q0::domains::tuple< q0::domains::cartesian<float,3> , q0::domains::vector<q0::domains::so2<float>> >");
	}

	{
		typedef q0::domains::tuple< q0::domains::vector<q0::domains::so2<float>> , q0::domains::cartesian<float,3> > domain_t;
		domain_t dom;
		std::get<0>(dom).resize(3);
		run<domain_t>(dom, "q0::domains::tuple< q0::domains::vector<q0::domains::so2<float>> , q0::domains::cartesian<float,3> >");
	}

	return 1;
}
