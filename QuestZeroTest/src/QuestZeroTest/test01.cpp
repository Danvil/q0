/*
 * test01.cpp
 *
 *  Created on: Sep 10, 2010
 *      Author: david
 */

#include <QuestZero/Optimization.h>
#include <QuestZero/IFunction.h>
#include <QuestZero/Algorithms/RND.h>
#include <QuestZero/Algorithms/PSO.h>
#include <QuestZero/Space/Cartesian.h>
#include <Danvil/LinAlg.h>
#include <boost/bind.hpp>

typedef Danvil::ctLinAlg::Vec<double,17> state17d;
typedef Danvil::ctLinAlg::Vec3f space3f;

typedef Spaces::Cartesian::CartesianSpace<state17d> space17d;

typedef FunctionFromDelegate<state17d> fnc17d;

double sphere(const state17d& s)
{
	double sum = 0;
	for(unsigned int i=0; i<s.dimension(); i++) {
		double x = s[i];
		sum += x * x;
	}
	return sum;
}

typedef Optimization<state17d, RND> rnd_solver;
typedef Optimization<state17d, PSO> pso_solver;

void foo()
{
	space17d myspace;
	state17d myrange;
	myrange.setAll(1);
	myspace.setDomainRange(myrange);

	PTR(fnc17d) myfnc(new fnc17d(boost::bind(&sphere, _1)));

	rnd_solver x;

	state17d result = x.optimize<space17d,fnc17d>(myspace, myfnc).state();

	TSample<state17d> u = Optimization<state17d, PSO>::Optimize(myspace, myfnc);
}
