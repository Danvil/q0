/*
 * VectorTraits.hpp
 *
 *  Created on: Jun 8, 2012
 *      Author: david
 */

#ifndef VECTORTRAITS_HPP_
#define VECTORTRAITS_HPP_

#include <Eigen/Dense>
#include <cassert>

namespace Q0
{

	/** A cartesian state vector has to provide the following operations:
	 * - state_t operator*(scalar_t, state_t) for multiplication of a vector by a scalar
	 * - state_t operator+(state_t, state_t) for addition of two vectors
	 * - state_t operator-(state_t) for unary negation of a vectors
	 * - scalar_t operator[](unsigned int i) for access of the i-th element
	 */

	template<typename K>
	struct VectorTraits
	{
		typedef K state_t;
		typedef K scalar_t;
		static const unsigned int dimension = 1;
		static state_t create(unsigned int dim) {
			assert(dim == dimension);
			return state_t();
		}
		static unsigned int dim(const state_t& x) {
			return 1;
		}
		static const scalar_t& at(const state_t& x, unsigned int i) {
			assert(i < dim(x));
			return x;
		}
		static scalar_t& at(state_t& x, unsigned int i) {
			assert(i < dim(x));
			return x;
		}
	};

	template<typename K, int N, int _Options, int _MaxRows, int _MaxCols>
	struct VectorTraits<Eigen::Matrix<K,N,1,_Options,_MaxRows,_MaxCols>>
	{
		typedef Eigen::Matrix<K,N,1,_Options,_MaxRows,_MaxCols> state_t;
		typedef K scalar_t;
		static const unsigned int dimension = N;
		static state_t create(unsigned int dim) {
			assert(dim == dimension);
			return state_t();
		}
		static unsigned int dim(const state_t& x) {
			return N;
		}
		static const scalar_t& at(const state_t& x, unsigned int i) {
			assert(i < dim(x));
			return x[i];
		}
		static scalar_t& at(state_t& x, unsigned int i) {
			assert(i < dim(x));
			return x[i];
		}
	};

	template<typename K, int _Options, int _MaxRows, int _MaxCols>
	struct VectorTraits<Eigen::Matrix<K,Eigen::Dynamic,1,_Options,_MaxRows,_MaxCols>>
	{
		typedef Eigen::Matrix<K,Eigen::Dynamic,1,_Options,_MaxRows,_MaxCols> state_t;
		typedef K scalar_t;
		static const unsigned int dimension = 0;
		static state_t create(unsigned int dim) {
			return state_t(dim);
		}
		static unsigned int dim(const state_t& x) {
			return x.rows();
		}
		static const scalar_t& at(const state_t& x, unsigned int i) {
			assert(i < dim(x));
			return x[i];
		}
		static scalar_t& at(state_t& x, unsigned int i) {
			assert(i < dim(x));
			return x[i];
		}
	};

}

#endif
