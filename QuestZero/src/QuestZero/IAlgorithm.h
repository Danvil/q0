#ifndef QUEST_ZERO_IALGORITHM
#define QUEST_ZERO_IALGORITHM

#include <Danvil/Ptr.h>

template<typename Problem/*, typename Settings*/>
class IMinimizationAlgorithm
{
public:
/*	class ISettings {
	public:
		unsigned int particleCount;
		unsigned int maxIterations;
	};

	Settings settings;*/

public:
	typedef typename Problem::State State;
	typedef typename Problem::Domain Domain;
	typedef typename Problem::Function Function;
	typedef typename Problem::Tracer Tracer;
	typedef TSampleSet<State> SampleSet;

public:
	virtual ~IMinimizationAlgorithm() {}

	virtual SampleSet Optimize2(
			boost::shared_ptr<Domain> dom,
			boost::shared_ptr<Function> fnc
	) {
		return OptimizeT(dom, fnc, Danvil::Ptr(new Tracer()));
	}

	virtual SampleSet OptimizeT(
			boost::shared_ptr<Domain> dom,
			boost::shared_ptr<Function> fnc,
			boost::shared_ptr<Tracer> tracer
	) {
		return OptimizeI(dom, fnc, std::vector<State>());
	}

	virtual SampleSet OptimizeI(
			boost::shared_ptr<Domain> dom,
			boost::shared_ptr<Function> fnc,
			const std::vector<State>& initial
	) {
		return Optimize(dom, fnc, initial, Danvil::Ptr(new Tracer()));
	}

	virtual TSampleSet<State> Optimize(
			boost::shared_ptr<Domain> dom,
			boost::shared_ptr<Function> fnc,
			const std::vector<State>& initial,
			boost::shared_ptr<Tracer> tracer
	) = 0;

};

#endif

