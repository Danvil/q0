/*
 * RandomNumbers.h
 *
 *  Created on: Sep 5, 2010
 *      Author: david
 */

#ifndef RANDOMNUMBERS_H_
#define RANDOMNUMBERS_H_

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <stdexcept>

class RandomNumbers
{
public:
	/** Random number in [a,b] */
	template<typename K>
	K random(K a, K b) const {
		throw std::runtime_error("Function K random(K,K) is not implemented for this type!");
	}

	/** Random number in [-a,+a] */
	template<typename K>
	K randomMP(const K a) const {
		return random(-a, +a);
	}

	/** Random number in [0,a] */
	template<typename K>
	K randomP(const K a) const {
		return random(0, a);
	}

	/** Random number in [0,1] */
	double random01() const {
		return random(0.0, 1.0);
	}

	/** Random number in [0,1] */
	unsigned int random(unsigned int a) const {
		return (unsigned int)random((int)0, (int)(a + 1));
	}

public:
	float random(float a, float b) const {
		boost::uniform_real<float> dist(a, b);
		boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > die(gen, dist);
		return die();
	}

	double random(double a, double b) const {
		boost::uniform_real<double> dist(a, b);
		boost::variate_generator<boost::mt19937&, boost::uniform_real<double> > die(gen, dist);
		return die();
	}

	int random(int a, int b) const {
		boost::uniform_int<int> dist(a, b);
		boost::variate_generator<boost::mt19937&, boost::uniform_int<int> > die(gen, dist);
		return die();
	}

private:
	// FIXME random number generator
	static boost::mt19937 gen;

public:
	static RandomNumbers S;
};

#endif
