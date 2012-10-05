#ifndef Q0_DOMAINS_TUPLE_H_
#define Q0_DOMAINS_TUPLE_H_
#include <q0/common.hpp>
#include <q0/math.hpp>
#include <q0/domains.hpp>
#include <boost/math/constants/constants.hpp>
#include <vector>
#include <tuple>
//---------------------------------------------------------------------------
namespace q0 { namespace domains {

using std::tuple;

/** The domain type is a std::tuple where the first parameter 
 * is the scalar type for the tangent space.
 * Use std::tuple<K, D1, D2, ...> as the domain type.
 * First parameter is the scalar type used for tangent space.
 */

template<typename K, typename... Args>
struct state_type<tuple<K, Args...>> {
	typedef tuple<K, typename state_type<Args>::type...> type;
};

template<typename K, typename... Args>
struct scalar_type<tuple<K, Args...>> {
	typedef K type;
};

/** Dimension of tangent space is not necessarily known at compile time  */
template<typename K, typename... Args>
struct tangent_type<std::tuple<K, Args...>> {
	typedef typename vecX<K>::type type;
};

namespace detail
{
	template<int N, int I=0>
	struct dimension_impl {
		template<typename K, typename... Args>
		static inline unsigned int apply(const tuple<K, Args...>& dom) {
			return dimension_impl<N,I+1>::apply(dom) + dimension(std::get<I+1>(dom)); // first is scalar type
		}
	};
	template<int N>
	struct dimension_impl<N,N> {
		template<typename K, typename... Args>
		static inline unsigned int apply(const tuple<K, Args...>& dom) {
			return 0;
		}
	};

	template<int N, int I=0>
	struct get_part_dimensions_impl {
		template<typename K, typename... Args>
		static inline void apply(const tuple<K, Args...>& dom) {
			return dimension_impl<N,I+1>::apply(dom) + dimension(std::get<I+1>(dom)); // first is scalar type
		}
	};
	template<int N>
	struct get_part_dimensions_impl<N,N> {
		template<typename K, typename... Args>
		static inline void apply(const tuple<K, Args...>& dom) {}
	};

	template<typename K, typename... Args>
	std::vector<unsigned int> get_part_dimensions(const tuple<K, Args...>& dom) {
		std::vector<unsigned int> dims(std::tuple_size<decltype(dom)>::value);

		return dims;
	}
}

template<typename K, typename... Args>
unsigned int dimension(const tuple<K, Args...>& dom) {
	return detail::dimension_impl<std::tuple_size<decltype(dom)>::value>::apply(dom);
}

namespae detail
{
	template<typename K>
	struct tangent_splitter {
		static constexpr unsigned int C = std::tuple_size<decltype(dom)>::value;
		std::array<std::pair<unsigned int,unsigned int>,C> parts;

		template<unsigned int I>
		vecX<K> get_tangent_part(const vecX<K>& x) const {
			return x.segment(parts[i].first, parts[i].second);
		}
	};

	std::vector<vecX<K>> split_tangent(const tuple<K, Args...>& dom, const vecX<K>& x) {
		std::vector<vecX<K>> parts(std::tuple_size<decltype(dom)>::value);

		return parts;
	}

	vecX<K> concatenate(const std::tuple<vecX<K>>& parts, unsigned int dim) {
		unsigned int p = 0;
		vecX<K> result(dim);
		for(const vecX<K>& u : parts) {
			result.segment(p, u.size()) = u;
			p += u.size();
		}
		return result;
	}
}

namespace detail
{
	template<int N, int I=0>
	struct for_each {
		template<typename <int> class F, typename... Args>
		static inline void apply(Args... args) {
			F<I>::apply(args);
			for_each<N,I+1>::apply<F>(args);
		}
	};

	template<int N>
	struct for_each<N,N> {
		template<typename <int> class F, typename... Args>
		static inline void apply(Args...) { }
	};

}

namespace detail
{
	// template<int N, int I=0>
	// struct exp_impl {
	// 	template<typename K, typename... Args>
	// 	static inline void apply(typename state_type<tuple<K, Args...>>::type& result, const tuple<K, Args...>& dom, const typename state_type<tuple<K, Args...>>::type& x, const typename tangent_type<tuple<K, Args...>>::type& y) {
	// 		std::get<I+1>(result) = exp(std::get<I>(dom), std::get<I>(x), detail::get_tangent_part(I, y));
	// 		return exp_impl<N,I+1>::apply(dom) + exp(std::get<I+1>(dom), std::get<I+1>(dom), std::get<I+1>(dom));
	// 	}
	// };
	// template<int N>
	// struct exp_impl<N,N> {
	// 	template<typename K, typename... Args>
	// 	static inline void apply(typename state_type<tuple<K, Args...>>::type& result, const tuple<K, Args...>& dom, const typename state_type<tuple<K, Args...>>::type& x, const typename tangent_type<tuple<K, Args...>>::type& y) {
	// 	}
	// };

	template<unsigned int I>
	struct exp_helper {
		static inline void apply(
			state_type<tuple<K, Args...>>::type& result,
			const tuple<K, Args...>& dom,
			const typename state_type<tuple<K, Args...>>::type& x,
			const std::vector<vecX<K>>& y_parts) 
		{
			std::get<I>(result) = exp(std::get<I>(dom), std::get<I>(x), y_parts[I]);
		}
	};

}

template<typename K, typename... Args>
typename state_type<tuple<K, Args...>>::type exp(const tuple<K, Args...>& dom, const typename state_type<tuple<K, Args...>>::type& x, const typename tangent_type<tuple<K, Args...>>::type& y) {
	state_type<tuple<K, Args...>>::type result;
	std::vector<vecX<K>> y_parts = details::split_tangent(dom, y);
	detail::for_each<std::tuple_size<decltype(dom)>::value>::apply<detail::exp_helper>(result, dom, x, y_parts);
	return result;
}

template<typename K, typename... Args>
typename tangent_type<tuple<K, Args...>>::type exp(const tuple<K, Args...>& dom, const typename state_type<tuple<K, Args...>>::type& x, const typename state_type<tuple<K, Args...>>::type& y) {
	// FIXME
	throw 0;
}

template<typename K, typename... Args>
typename state_type<tuple<K, Args...>>::type random(const tuple<K, Args...>& dom) {
	// FIXME
	throw 0;
}

template<typename K, typename... Args>
typename state_type<tuple<K, Args...>>::type random_neighbour(const tuple<K, Args...>& dom, const typename state_type<tuple<K, Args...>>::type& x, K radius) {
	// FIXME
	throw 0;
}

template<typename K, typename... Args>
typename state_type<tuple<K, Args...>>::type mean(const tuple<K, Args...>& dom, const std::vector<K>& weights, const std::vector<typename state_type<tuple<K, Args...>>::type>& states) {
	// FIXME
	throw 0;
}

}}
//---------------------------------------------------------------------------
#endif
