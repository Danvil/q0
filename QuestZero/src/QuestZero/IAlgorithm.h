#ifndef QUEST_ZERO_IALGORITHM
#define QUEST_ZERO_IALGORITHM

#include <Danvil/Ptr.h>
#include <boost/foreach.hpp>

template<typename Traits>
class IFunction
{
public:
	virtual double operator()(const typename Traits::State& state) = 0;
};

template<typename Traits>
class IMinimizationAlgorithm
{
public:
	virtual ~IMinimizationAlgorithm() {}

	virtual typename Traits::SampleSet Optimize(boost::shared_ptr<IFunction<Traits> > f) = 0;

};

#endif

