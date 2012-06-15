/*
 * music.cpp
 *
 * This example generates a random melody by optimizing
 * a shady harmony objective function.
 *
 * See for example http://tones.wolfram.com/generate/
 * for more fun with random music.
 *
 *  Created on: Jun 14, 2012
 *      Author: David Weikersdorfer
 */

#include <QuestZero/Spaces/Multiplier.h>
#include <QuestZero/Optimization/HarmonySearch.h>
#include <QuestZero/Optimization/RND.h>
//#include <QuestZero/Optimization/PSO.h>
#include <QuestZero/Optimization/Optimization.h>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>

struct NoteState
{
	unsigned int value;

	int frequency() const {
		static int frq[12] = {264, 275, 297, 317, 330, 352, 367, 396, 422, 440, 475, 495 };
		return frq[value % 12] * (value < 12 ? 1 : 2);
	}

	void print(std::ostream& os) const {
		static const char* names[24] = {
				"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "H",
				"c", "c#", "d", "d#", "e", "f", "f#", "g", "g#", "a", "a#", "h"
		};
		os << names[value % 24];
	}
};

std::ostream& operator<<(std::ostream& os, const NoteState& note) {
	note.print(os);
	return os;
}

boost::mt19937 gen;

struct NoteSpace
{
	unsigned int dimension() const {
		return 1;
	}

	NoteState random() {
		boost::uniform_int<> dist(0, 24);
		boost::variate_generator<boost::mt19937&, boost::uniform_int<> > die(gen, dist);
		return { die() };
	}

	void component_copy(NoteState& dst, unsigned int cid, const NoteState& src) const {
		assert(cid == 0);
		dst = src;
	}

	void component_add_noise(NoteState& dst, unsigned int cid, double noise) const {
		assert(cid == 0);
		dst.value += static_cast<int>(std::floor(Q0::RandomNumbers::Uniform(-noise, +noise) + 0.5));
	}

};

typedef Q0::Spaces::MultiplierState<NoteState> state_t;

typedef Q0::Spaces::MultiplierSpace<NoteSpace, state_t> space_t;

typedef double score_t;

int harmony_pair_objective(int ta, int tb) {
	int d = std::abs(ta - tb);
	int d_h = d % 12;
	d_h = std::min(d_h, 12 - d_h);
	int q[] = { std::abs(d_h - 4), std::abs(d_h - 7) };
	int q_min = *std::min_element(q, q+2);
	int t = std::max(d - 12, 0);
	return q_min * q_min + t * t;
}

// the function to optimize
score_t harmony_objective(state_t x)
{
	int total = 0;
	for(unsigned int i=0; i<x.count(); i++) {
		total += harmony_pair_objective(x[i].value, x[(i+1)%x.count()].value);
		total += harmony_pair_objective(x[i].value, x[(i+2)%x.count()].value) / 2;
		total += harmony_pair_objective(x[i].value, x[(i+7)%x.count()].value) / 4;
	}
	return total;
}

/** Prints samples for debug purposes */
struct DebugVisitor
{
	DebugVisitor(int verbose=0)
	: verbose_(verbose) {}

	template<typename SampleSet>
	void NotifySamples(const SampleSet& samples) {
		if(verbose_ >= 1) {
			auto id_best = Q0::find_best_by_score(samples, std::less<score_t>());
			std::cout << "Current Best = " << Q0::get_state(samples, id_best) << std::endl;
			if(verbose_ >= 2) {
				std::cout << "Current samples" << std::endl;
				for(auto id : Q0::samples(samples)) {
					std::cout << Q0::get_score(samples, id) << " : " << Q0::get_state(samples, id) << std::endl;
				}
			}
		}
	}

private:
	int verbose_;
};

/** Write simple midi file */
void write_midi(const std::string& fn, const state_t& music);

int main(int argc, char** argv)
{
	std::string p_algo_name = "HarmonySearch";
	int p_verbose = 0;
	unsigned int p_total_evaluations = 100000;

	// read parameters from command line
	{
		using namespace std;
		namespace po = boost::program_options;
		po::options_description desc("Allowed options");
		desc.add_options()
		    ("help", "produce help message")
		    ("algo", po::value<std::string>(&p_algo_name), "optimization algorithm")
		    ("verbosity", po::value<int>(&p_verbose), "verbosity 0-2")
		    ("evaluations", po::value<unsigned int>(&p_total_evaluations), "number of evaluations")
		;

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.count("help")) {
		    cout << desc << "\n";
		    return 1;
		}
	}

	// create score space
	space_t space(Q0::Spaces::MultiplierSizePolicies::DynamicSize(12));
	gen.seed(time(NULL));

	// prepare and run optimization
	Q0::Sample<state_t, score_t> best;
	if(p_algo_name == "RND") {
		Q0::Optimization<state_t, score_t,
				Q0::RND,
				Q0::InitializePolicy::RandomPicker<state_t>,
				Q0::ExitPolicy::FixedChecks<score_t,true>
		> algo;
		algo.SetIterationCount(p_total_evaluations / 100);
		algo.particleCount = 100;
		best = algo.Minimize(space, &harmony_objective, DebugVisitor(p_verbose));
	}
	else if(p_algo_name == "HarmonySearch") {
		Q0::Optimization<state_t, score_t,
				Q0::HarmonySearch,
				Q0::InitializePolicy::RandomPicker<state_t>,
				Q0::ExitPolicy::FixedChecks<score_t,true>
		> algo;
		algo.SetIterationCount(p_total_evaluations);
		algo.parameters_.fw = 1.4;
		best = algo.Minimize(space, &harmony_objective, DebugVisitor(p_verbose));
	}
//	else if(p_algo_name == "PSO") {
//		Q0::Optimization<state_t, score_t,
//				Q0::PSO,
//				Q0::InitializePolicy::RandomPicker<state_t>,
//				Q0::ExitPolicy::FixedChecks<score_t,true>
//		> algo;
//		algo.SetIterationCount(p_total_evaluations / 30);
//		algo.settings.particleCount = 30;
//		best = algo.Minimize(space, &harmony_objective, DebugVisitor(p_verbose));
//	}
	else {
		std::cerr << "Invalid algorithm '" << p_algo_name << "'" << std::endl;
	}

	// print results
	std::cout << "Optimization result:" << std::endl;
	std::cout << "x_min = " << best.state << std::endl;
	std::cout << "f(x_min) = " << best.score << std::endl;

	// save and play midi
	write_midi("result.mid", best.state);
	return system("totem result.mid");
}

void write_midi(const std::string& fn, const state_t& music) {
	std::ofstream ofs(fn, std::ofstream::binary);
	// 4 -> 60
	// 12 -> 140
	// offset 93
	//int header[] = { 77, 84, 104, 100, 0, 0, 0, 6, 0, 1, 0, 2, 1, 224, 77, 84, 114, 107, 0, 0, 0, 118, 0, 255, 2, 35, 67, 111, 112, 121, 114, 105, 103, 104, 116, 32, 40, 99, 41, 32, 120, 120, 120, 120, 32, 67, 111, 112, 121, 114, 105, 103, 104, 116, 32, 72, 111, 108, 100, 101, 114, 0, 255, 7, 21, 67, 114, 101, 97, 116, 101, 100, 32, 98, 121, 32, 82, 111, 115, 101, 103, 97, 114, 100, 101, 110, 0, 255, 7, 31, 104, 116, 116, 112, 58, 47, 47, 119, 119, 119, 46, 114, 111, 115, 101, 103, 97, 114, 100, 101, 110, 109, 117, 115, 105, 99, 46, 99, 111, 109, 47, 0, 255, 81, 3, 7, 161, 32, 0, 255, 88, 4, 4, 2, 24, 8, 0, 255, 47, 0, 77, 84, 114, 107, 0, 0, 0,  60, 0, 255, 3, 0, 0, 192, 0, 0, 176, 7, 100, 0, 10, 64, 0, 93, 0, 0, 91, 0, 0 };
	  int header[] = { 77, 84, 104, 100, 0, 0, 0, 6, 0, 1, 0, 2, 1, 224, 77, 84, 114, 107, 0, 0, 0, 118, 0, 255, 2, 35, 67, 111, 112, 121, 114, 105, 103, 104, 116, 32, 40, 99, 41, 32, 120, 120, 120, 120, 32, 67, 111, 112, 121, 114, 105, 103, 104, 116, 32, 72, 111, 108, 100, 101, 114, 0, 255, 7, 21, 67, 114, 101, 97, 116, 101, 100, 32, 98, 121, 32, 82, 111, 115, 101, 103, 97, 114, 100, 101, 110, 0, 255, 7, 31, 104, 116, 116, 112, 58, 47, 47, 119, 119, 119, 46, 114, 111, 115, 101, 103, 97, 114, 100, 101, 110, 109, 117, 115, 105, 99, 46, 99, 111, 109, 47, 0, 255, 81, 3, 7, 161, 32, 0, 255, 88, 4, 4, 2, 24, 8, 0, 255, 47, 0, 77, 84, 114, 107, 0, 0, 0, 140, 0, 255, 3, 0, 0, 192, 0, 0, 176, 7, 100, 0, 10, 64, 0, 93, 0, 0, 91, 0, 0 };
//	int mid[] = {
//			144, 67, 100, 131, 96, 128, 67, 127, 0,
//			144, 71, 100, 131, 96, 128, 71, 127, 0,
//			144, 74, 100, 131, 96, 128, 74, 127, 0,
//			144, 77, 100, 131, 96, 128, 77, 127, 0};
	int extro[] = { 255, 47, 0 };
	for(int x : header) {
		char c = x;
		ofs.write(&c, 1);
	}
	for(unsigned int i=0; i<1; i++) {
		// extra foo for first if 12
		int mid[] = { 144, 67, 100,  0, 255, 3, 0, 0, 255, 3, 0,  131, 96, 128, 67, 127, 0, };
		int q = 60 + music[i].value;
		mid[1] = q;
		mid[8+6] = q;
		for(int x : mid) {
			char c = x;
			ofs.write(&c, 1);
		}
	}
	for(unsigned int i=1; i<12; i++) {
		//NoteState x = music[i];
		int mid[] = { 144, 67, 100, 131, 96, 128, 67, 127, 0, };
		int q = 60 + music[i].value;
		mid[1] = q;
		mid[6] = q;
		for(int x : mid) {
			char c = x;
			ofs.write(&c, 1);
		}
	}
	for(int x : extro) {
		char c = x;
		ofs.write(&c, 1);
	}
}

