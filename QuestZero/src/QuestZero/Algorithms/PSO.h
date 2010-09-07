/*
 * PSO.h
 *
 *  Created on: Sep 5, 2010
 *      Author: david
 */

#ifndef QUESTZERO_ALGORITHMS_PSO_H_
#define QUESTZERO_ALGORITHMS_PSO_H_

#include "../StartingValues.h"
#include "../SampleSet.h"
#include "../IAlgorithm.h"
#include <Danvil/Ptr.h>
#include <vector>
#include <string>
#include <stdexcept>
#include <cassert>

/// <summary>
/// The particle swarm algorithm
/// See http://en.wikipedia.org/wiki/Particle_swarm_optimization
/// See http://www.hvass-labs.org/projects/swarmops/
/// See Particle Swarm Optimization: Developments, Application and Ressources, Eberhart, R. and Shi, Y.
/// </summary>
template<typename Problem>
class PSO
: public IMinimizationAlgorithm<Problem>
{
public:
	typedef typename Problem::State State;
	typedef TSample<State> Sample;
	typedef TSampleSet<State> SampleSet;
	typedef typename Problem::StateOperator Op;
	typedef typename Problem::Domain Domain;
	typedef typename Problem::Function Function;
	typedef typename Problem::Tracer Tracer;

public:
	std::string name() { return "PSO"; }

	class PSOSettings
	{
	public:
		PSOSettings() {
			particleCount = 200;
			factor_velocity = 1.0;
			factor_personal = 2.05;
			factor_global = 2.05;
		}
		unsigned int iterations;
		unsigned int particleCount;
		double factor_velocity;
		double factor_personal;
		double factor_global;
	};

	PSOSettings settings;

	PSO() {}

	virtual ~PSO() {}

	SampleSet Optimize(PTR(Domain) dom, PTR(Function) fnc, const std::vector<State>& given_initial_states, PTR(Tracer) tracer) {
		globals.set(settings);
		globals._domain = dom;
		// generate start samples
		std::vector<State> complete_initial_states = StartingValues::Repeat(given_initial_states, settings.particleCount);
		SampleSet initial(complete_initial_states);
		BOOST_FOREACH(const Sample& s, initial.samples()) {
			particles.push_back(ParticleData(s.state()));
		}
		// iterate
		for(unsigned int k = 0; k < settings.iterations; k++) {
			// construct sample set
			SampleSet samples = currentSamples();
			// evaluate samples
			samples.evaluateUnknown(fnc);
			// update global best
			const Sample& best = samples.best();
			if(!globals.isSet() || best.score() < globals.bestScore()) {
				globals.set(best);
			}
			// update personal best and particle
			for(size_t i = 0; i < particles.size(); i++) {
				const Sample& s = samples.samples()[i];
				ParticleData& p = particles[i];
				if(s.score() < p.best_score) {
					p.best = s.state();
					p.best_score = s.score();
				}
				p.Update(globals);
			}
			// trace
			tracer->trace(k + 1, settings.iterations, bestSamples());
			// check if best satisfy break condition
//			if(Settings.Finished(k + 1, globals.best_score))
//				break;
		}
		return bestSamples();
	}

private:
	class GlobalData
	{
	public:
		GlobalData()
		: _isBestSet(false) {}

		void set(PSOSettings x) {
			_omega = x.factor_velocity;
			_psi_personal = x.factor_personal;
			_psi_global = x.factor_global;
			double K = computeK();
			_omega *= K;
			_psi_personal *= K;
			_psi_global *= K;
			_best_score = 1e12;
		}

		double omega() const { return _omega; }

		double psi_personal() const { return _psi_personal; }

		double psi_global() const { return _psi_global; }

		bool isSet() const { return _isBestSet; }

		double bestScore() const { return _best_score; }

		const State& best() const { return _best; }

		void set(const Sample& s) {
			_best = s.state();
			_best_score = s.score();
			_isBestSet = true;
		}

	private:
		double _omega;
		double _psi_personal;
		double _psi_global;

	private:
		/// <summary>
		/// Constriction Factor (see Clerc, 1999)
		/// </summary>
		/// <returns></returns>
		double computeK() {
			double u = _psi_personal + _psi_global;
			if(u <= 4) {
				throw std::runtime_error("psi_personal + psi_global must be > 4!");
			}
			return 2 / abs(2 - u - sqrt(u * u - 4 * u));
		}

	private:
		bool _isBestSet;
		State _best;
		double _best_score;

	public:
		PTR(Domain) _domain;
	};

	class ParticleData
	{
	public:
		ParticleData(State x) {
			last = x;
			current = x;
			best = x;
			best_score = 1e12;
		}

		State last;
		State current;
		State best;
		double best_score;

		void Update(GlobalData globals) {
			double fl = globals.omega();
			double fp = globals.psi_personal() * RandomNumbers::S.random01();
			double fg = globals.psi_global() * RandomNumbers::S.random01();
			State dl = Op::Difference(current, last);
			State dp = Op::Difference(best, current);
			State dg = Op::Difference(globals.best(), current);
			last = current;
			State delta = Op::WeightedSum(fl, dl, fp, dp, fg, dg);
			// FIXME we should bound the "velocity" i.e. the maximal distance made in a step
			current = globals._domain->project(Op::Compose(delta, current));
		}
	};

private:
	std::vector<ParticleData> particles;

	GlobalData globals;

	SampleSet currentSamples() const {
		std::vector<State> states;
		BOOST_FOREACH(const ParticleData& p, particles) {
			states.push_back(p.current);
		}
		return SampleSet(states);
	}

	SampleSet bestSamples() const {
		std::vector<Sample> samples;
		BOOST_FOREACH(const ParticleData& p, particles) {
			samples.push_back(Sample(p.best, p.best_score));
		}
		return SampleSet(samples);
	}

};

#endif
