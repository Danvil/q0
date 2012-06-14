/*
 * music.cpp
 *
 *  Created on: Jun 14, 2012
 *      Author: david
 */

#include <QuestZero/Spaces/Multiplier.h>
#include <QuestZero/Optimization/experimental/HarmonySearch.h>
#include <QuestZero/Optimization/PSO.h>
#include <QuestZero/Optimization/Optimization.h>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>
#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>

boost::mt19937 gen;

struct NoteState
{
	unsigned int value;

	int frequency() const {
		static int frq[12] = { 264, 275, 297, 317, 330, 352, 367, 396, 422, 440, 475, 495 };
		return frq[value % 12];
	}

	void print(std::ostream& os) const {
		static const char* names[12] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "H" };
		os << names[value % 12];
	}
};

std::ostream& operator<<(std::ostream& os, const NoteState& note) {
	note.print(os);
	return os;
}

struct NoteSpace
{
	unsigned int dimension() const {
		return 1;
	}

	NoteState random() {
		boost::uniform_int<> dist(0, 12);
		boost::variate_generator<boost::mt19937&, boost::uniform_int<> > die(gen, dist);
		return { die() };
	}

//	double distance(const NoteState& x, const NoteState& y) const {
//	}
//
//	NoteState scale(const NoteState& a, double s) const {
//	}
//
//	NoteState inverse(const NoteState& a) const {
//	}
//
//	NoteState compose(const NoteState& a, const NoteState& b) const {
//	}
//
//	NoteState difference(const NoteState& a, const NoteState& b) const {
//	}
//
//	template<typename K>
//	NoteState weightedSum(K f1, const NoteState& s1, K f2, const NoteState& s2) const {
//	}
//
//	template<typename K>
//	NoteState weightedSum(K f1, const NoteState& s1, K f2, const NoteState& s2, K f3, const NoteState& s3) const {
//	}
//
//	template<typename K>
//	NoteState weightedSum(const std::vector<K>& factors, const std::vector<NoteState>& states) const {
//	}

	void component_copy(NoteState& dst, unsigned int cid, const NoteState& src) const {
		assert(cid == 0);
		dst = src;
	}

	void component_add_noise(NoteState& dst, unsigned int cid, double noise) const {
		assert(cid == 0);
		dst.value += static_cast<int>(Q0::RandomNumbers::Uniform(-noise, +noise));
	}
};

typedef Q0::Spaces::MultiplierState<NoteState> state_t;

typedef Q0::Spaces::MultiplierSpace<NoteSpace, state_t> space_t;

typedef double score_t;

void write_midi(const std::string& fn, const state_t& music);

// the function to optimize
score_t harmony_objection(state_t x) {
	return 0;
}

int main(int argc, char** argv)
{
	gen.seed(time(NULL));

	space_t space(Q0::Spaces::MultiplierSizePolicies::DynamicSize(12));

//	Q0::Optimization<state_t, score_t,
//			Q0::PSO,
//			Q0::InitializePolicy::RandomPicker<state_t>,
//			Q0::ExitPolicy::FixedChecks<score_t,true>
//	> algo;

	Q0::Optimization<state_t, score_t,
			Q0::HarmonySearch,
			Q0::InitializePolicy::RandomPicker<state_t>,
			Q0::ExitPolicy::FixedChecks<score_t,true>
	> algo;

	// setup algorithm to do 5 iterations
	algo.SetIterationCount(5);

//	// setup algorithm to use 100 particles
//	algo.settings.particleCount = 100;
	algo.parameters_.fw = 3;

	// find minimum
	Q0::Sample<state_t, score_t> best = algo.Minimize(space, &harmony_objection);

	// print results
	std::cout << "Optimization result:" << std::endl;
	std::cout << "x_min = " << best.state << std::endl;
	std::cout << "f(x_min) = " << best.score << std::endl;

	write_midi("result.mid", best.state);

	int i = system("totem result.mid");
	return i;
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

