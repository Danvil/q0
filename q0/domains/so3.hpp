#ifndef Q0_DOMAINS_SO3_H_
#define Q0_DOMAINS_SO3_H_
#include <q0/common.hpp>
#include <q0/math.hpp>
#include <q0/domains.hpp>
#include <boost/math/constants/constants.hpp>
#include <Eigen/Dense>
#include <vector>
//---------------------------------------------------------------------------
namespace q0 { namespace domains {

/**
 * Definitions:
 * state domain: full so(3)
 * state type: quaternion q
 * tangent type: rodriguez vector r
 * q = qw,qx,qz,qy
 * r = rx,ry,rz
 * Conversion:
 * q_w = cos(a/2)
 * q_xyz = sin(a/2) * q_xyz^0
 * r = q_xyz * atan2(||q_xyz||,qw)
 * a = ||r||
 */ 

template<typename K>
struct so3 {};

template<typename K>
struct tangent_size<so3<K>> : std::integral_constant<int,3> {};

template<typename K>
struct state_type<so3<K>> {
	// using quaternions
	typedef Eigen::Quaternion<K> type;
};

template<typename K>
void print(std::ostream& os, const so3<K>&, const typename state_type<so3<K>>::type& u) {
	os << "so(3){" << u.w() << "," << u.x() << "," << u.y() << "," << u.z() << "}";
}

template<typename K>
typename state_type<so3<K>>::type restrict(const so3<K>&, const typename state_type<so3<K>>::type& x) {
	return x.normalized();
}

template<typename T, typename K>
typename state_type<so3<K>>::type exp(const so3<K>& dom, const typename state_type<so3<K>>::type& y, const typename tangent_type<T,so3<K>>::type& t) {
	K n = t.norm();
	if(n == K(0)) {
		return y;
	}
	K a = static_cast<K>(0.5)*n;
	K ca = std::cos(a);
	K sa = std::sin(a);
	K scl = sa / n;
	return restrict(dom, y * Eigen::Quaternion<K>(ca, scl*t[0], scl*t[1], scl*t[2]));
}

template<typename T, typename K>
typename tangent_type<T,so3<K>>::type log(const so3<K>&, const typename state_type<so3<K>>::type& y, const typename state_type<so3<K>>::type& x) {
	// tangent is 1-dim vector -> need to initialize correctly;
	typename tangent_type<T,so3<K>>::type v;
	typename state_type<so3<K>>::type d = y.inverse() * x;
	K sa = d.vec().norm();
	K scl = (sa == K(0) ? 0 : (2.0f*std::atan2(sa, d.w())/sa));
	return (scl*d.vec()).template cast<T>();
}

template<typename K>
typename state_type<so3<K>>::type random(const so3<K>& dom) {
	// quaternions are 4D unit sphere
	// FIXME this may fail when we get four times 0 ...
	return restrict(dom, Eigen::Quaternion<float>{
		math::random_uniform<K>(K(-1), K(+1)),
		math::random_uniform<K>(K(-1), K(+1)),
		math::random_uniform<K>(K(-1), K(+1)),
		math::random_uniform<K>(K(-1), K(+1)) });
}

template<typename K>
typename state_type<so3<K>>::type random_neighbour(const so3<K>& dom, const typename state_type<so3<K>>::type& x, double radius) {
	typedef K T;
	typedef typename tangent_type<T,so3<K>>::type t_t;
	K r = radius;
	// TODO do we know a direct method to avoid the tangent space?
	// TODO what is the meaning of "radius"
	return exp<T,K>(dom, x, t_t{
		math::random_uniform<K>(-r, +r),
		math::random_uniform<K>(-r, +r),
		math::random_uniform<K>(-r, +r)
	});
}

template<typename W, typename K>
typename state_type<so3<K>>::type mean(const so3<K>& dom, const std::vector<W>& weights, const std::vector<typename state_type<so3<K>>::type>& states) {
	constexpr unsigned MAX_ITER = 1000;
	constexpr K TARGET = K(0.001);
	// using general iterative algorithm
	typedef W T;
	typedef typename tangent_type<T,so3<K>>::type t_t;
	typedef typename state_type<so3<K>>::type s_t;
	size_t n = weights.size();
	if(n == 0) {
		std::cerr << "so3::mean does not work with 0 elements!" << std::endl;
		throw 0; // FIXME
	}
	if(n == 1) {
		return states[0];
	}
	BOOST_ASSERT(n == states.size());
	// initial value
	s_t id = s_t::Identity();
	t_t logy = weights[0]*log<T>(dom, id, states[0]);
	for(size_t i=1; i<n; i++) {
		logy += weights[i]*log<T>(dom, id, states[i]);
	}
	s_t y = exp<T>(dom, id, logy);
	if(y.norm() == K(0)) {
		std::cerr << "so3::mean ill posed problem as mean of elements is 0!" << std::endl;
		throw 0; // FIXME
	}
	y = restrict(dom, y);
	// iterative algorithm
	for(unsigned k=0; ; k++) {
		t_t e = t_t::Zero();
		for(size_t i=0; i<n; i++) {
			e += weights[i]*log<T>(dom, y, states[i]);
		}
		// check end
		if(e.squaredNorm() <= TARGET*TARGET) {
			break;
		}
		if(k >= MAX_ITER) {
			std::cerr << "so3::mean did not converge after " << MAX_ITER << " iterations!" << std::endl;
			throw 0; // FIXME
		}
		// new value
		y = exp<T>(dom, y, e);
	}
	return y;
}

template<typename W, typename K>
typename state_type<so3<K>>::type lerp(const so3<K>& dom, W p, const typename state_type<so3<K>>::type& u, const typename state_type<so3<K>>::type& v) {
	return mean<W,K>(dom, {1-p,p}, {u,v});
}

}}
//---------------------------------------------------------------------------
#endif
