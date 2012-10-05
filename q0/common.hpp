#ifndef Q0_COMMON_HPP_
#define Q0_COMMON_HPP_
#include <Eigen/Dense>
#include <boost/assert.hpp>
#include <vector>
#include <algorithm>
//---------------------------------------------------------------------------
namespace q0 {

template<typename K, int N>
struct vec
{
	typedef Eigen::Matrix<K,N,1> type;
};

template<typename K>
struct vec<K,-1>
{
	typedef Eigen::Matrix<K,Eigen::Dynamic,1> type;
};

template<typename K>
struct vecX
{
	typedef typename vec<K,-1>::type type;
};

template<typename K, int N>
K at(const Eigen::Matrix<K,N,1>& v, unsigned int i) {
	return v[i];
}

template<typename K, int N>
K& at(Eigen::Matrix<K,N,1>& v, unsigned int i) {
	return v[i];
}

template<typename State, typename Score>
struct particle
{
	State state;
	Score score;
};

}
//---------------------------------------------------------------------------
#endif
