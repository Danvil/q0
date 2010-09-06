#ifndef QUEST_ZERO_IALGORITHM
#define QUEST_ZERO_IALGORITHM

#include <Danvil/Ptr.h>

template<typename Problem>
class IMinimizationAlgorithm
{
public:
	virtual ~IMinimizationAlgorithm() {}

	virtual TSampleSet<typename Problem::State> Optimize2(
			boost::shared_ptr<typename Problem::Domain> dom,
			boost::shared_ptr<typename Problem::Function> fnc
	) {
		return Optimize(dom, fnc, Danvil::Ptr(new typename Problem::Tracer()));
	}

	virtual TSampleSet<typename Problem::State> Optimize(
			boost::shared_ptr<typename Problem::Domain> dom,
			boost::shared_ptr<typename Problem::Function> fnc,
			boost::shared_ptr<typename Problem::Tracer> tracer
	) = 0;

};

#endif

