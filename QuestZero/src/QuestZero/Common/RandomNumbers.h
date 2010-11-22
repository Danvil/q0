/*
 * RandomNumbers.h
 *
 *  Created on: Sep 5, 2010
 *      Author: david
 */

#ifndef RANDOMNUMBERS_H_
#define RANDOMNUMBERS_H_
//---------------------------------------------------------------------------
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <stdexcept>
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

namespace RandomNumbers
{

	inline
	boost::mt19937& Generator() {
		static boost::mt19937 generator;
		return generator;
	}

	template<typename DIST>
	typename DIST::result_type Generate(const DIST& dist) {
		boost::variate_generator<boost::mt19937&, DIST> die(Generator(), dist);
		return die();
	}

	/** Random real in [0,1] */
	template<typename K>
	K Uniform() {
		return Generate(boost::uniform_real<K>(0, 1));
	}

	/** Random real in [0,range] */
	template<typename K>
	K Uniform(K range) {
		return Generate(boost::uniform_real<K>(0, range));
	}

	/** Random real in [a,b] */
	template<typename K>
	K Uniform(K a, K b) {
		return Generate(boost::uniform_real<K>(a, b));
	}

	/** Random real in [-range,+range] */
	template<typename K>
	K UniformMP(K range) {
		return Generate(boost::uniform_real<K>(-range, range));
	}

	/** Random integer in [0,range] */
	template<typename K>
	K Index(K range) {
		return Generate(boost::uniform_int<K>(0, range));
	}

	/** Random integer in [a,b] */
	template<typename K>
	K Index(K a, K b) {
		return Generate(boost::uniform_int<K>(a, b));
	}

};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
