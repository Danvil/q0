#ifndef QUEST_ZERO_IALGORITHM
#define QUEST_ZERO_IALGORITHM

#include <Danvil/Ptr.h>
#include <boost/foreach.hpp>
//#include <iostream>
//using std::cout;
//using std::endl;

template<typename State>
class IFunction
{
public:
	virtual double operator()(const State& state) = 0;
};

template<typename Traits>
class IMinimizationAlgorithm
{
public:
	virtual ~IMinimizationAlgorithm() {}

	virtual typename Traits::SampleSet Optimize(
			boost::shared_ptr<typename Traits::Domain> dom,
			boost::shared_ptr<typename Traits::Function> f
	) = 0;

	void Trace(int i, int total, const typename Traits::SampleSet& samples) {
//		cout << "Round " << i << "/" << total << ": best=" << samples << endl;
	}

};

#endif

