#ifndef Q0_DOMAINS_TUPLE_H_
#define Q0_DOMAINS_TUPLE_H_
#include <q0/common.hpp>
#include <q0/math.hpp>
#include <q0/domains.hpp>
#include <vector>
#include <tuple>
//---------------------------------------------------------------------------
namespace q0 { namespace domains {

/** The domain type is a std::tuple */
using std::tuple;

/** State type is a tuple of the resp state types */
template<typename... Args>
struct state_type<tuple<Args...>> {
	typedef tuple<typename state_type<Args>::type...> type;
};

namespace detail
{
	template<unsigned int>
	struct int_ {};

	template<unsigned int N, unsigned int I, template<unsigned int> class F>
	struct for_each_impl {
		template<typename... Args>
		static inline void apply(Args... args) {
			F<I>::apply(args...);
			for_each_impl<N,I+1,F>::apply(args...);
		}
	};

	template<unsigned int N, template<unsigned int> class F>
	struct for_each_impl<N,N,F> {
		template<typename... Args>
		static inline void apply(Args... args) { }
	};

	template<unsigned int N, template<unsigned int> class F, typename... Args>
	static inline void for_each(Args... args) {
		for_each_impl<N,0,F>::apply(args...);
	}
}

namespace detail
{
	template<unsigned int I>
	struct print_helper {
		template<typename... Args>
		static inline void apply(std::ostream* os, const tuple<Args...>& dom, const typename state_type<tuple<Args...>>::type& u) {
			if(I > 0) {
				*os << ", ";
			}
			*os << static_cast<unsigned char>('a'+I) << "=";
			print(*os, std::get<I>(dom), std::get<I>(u));
		}
	};
}

template<typename... Args>
void print(std::ostream& os, const tuple<Args...>& dom, const typename state_type<tuple<Args...>>::type& u) {
	os << "tuple< ";
	detail::for_each<std::tuple_size<tuple<Args...>>::value, detail::print_helper>(&os, dom, u);
	os << " >";
}

namespace detail
{
	template<unsigned int I>
	struct dimension_helper {
		template<typename... Args>
		static inline void apply(unsigned int* result, const tuple<Args...>& dom) {
			*result += dimension(std::get<I>(dom));
		}
	};
}

template<typename... Args>
unsigned int dimension(const tuple<Args...>& dom) {
	unsigned int result = 0;
	detail::for_each<std::tuple_size<tuple<Args...>>::value, detail::dimension_helper>(&result, dom);
	return result;
}

namespace detail
{
	// template<int N, int I=0>
	// struct get_part_dimensions_impl {
	// 	template<typename... Args>
	// 	static inline void apply(const tuple<Args...>& dom) {
	// 		return dimension_impl<N,I+1>::apply(dom) + dimension(std::get<I+1>(dom)); // first is scalar type
	// 	}
	// };
	// template<int N>
	// struct get_part_dimensions_impl<N,N> {
	// 	template<typename... Args>
	// 	static inline void apply(const tuple<Args...>& dom) {}
	// };

	// template<typename... Args>
	// std::vector<unsigned int> get_part_dimensions(const tuple<Args...>& dom) {
	// 	std::vector<unsigned int> dims(std::tuple_size<decltype(dom)>::value);

	// 	return dims;
	// }

	// template<typename K>
	// struct tangent_splitter {
	// 	static constexpr unsigned int C = std::tuple_size<decltype(dom)>::value;
	// 	std::array<std::pair<unsigned int,unsigned int>,C> parts;

	// 	template<unsigned int I>
	// 	vecX<K> get_tangent_part(const vecX<K>& x) const {
	// 		return x.segment(parts[i].first, parts[i].second);
	// 	}
	// };

	// std::vector<vecX<K>> split_tangent(const tuple<Args...>& dom, const vecX<K>& x) {
	// 	std::vector<vecX<K>> parts(std::tuple_size<decltype(dom)>::value);

	// 	return parts;
	// }

	template<typename... Args>
	unsigned int get_tangent_part_position(int_<0>, const tuple<Args...>& dom) {
		return 0;
	}

	template<unsigned int I, typename... Args>
	unsigned int get_tangent_part_position(int_<I>, const tuple<Args...>& dom) {
		return get_tangent_part_position(int_<I-1>(), dom) + dimension(std::get<I-1>(dom));
	}

	template<unsigned int I, typename T, typename... Args>
	typename tangent_type<T,typename std::tuple_element<I,tuple<Args...>>::type>::type get_tangent_part(const tuple<Args...>& dom, const typename tangent_type<T,tuple<Args...>>::type& t) {
		const unsigned int pos = get_tangent_part_position(int_<I>(), dom);
		BOOST_ASSERT(pos < dimension(dom));
		const unsigned int dim = dimension(std::get<I>(dom));
		BOOST_ASSERT(t.size() == dimension(dom));
//		constexpr int ts = tangent_size<tuple<Args...>>::value;
//		if(ts == -1) {
			return t.segment(pos, dim);
//		}
//		else {
//			return t.template segment<ts>(pos);
//		}
	}

	template<unsigned int I, typename T, typename... Args>
	void set_tangent_part(const tuple<Args...>& dom, typename tangent_type<T,tuple<Args...>>::type& t, const typename tangent_type<T,typename std::tuple_element<I,tuple<Args...>>::type>::type& p) {
		const unsigned int pos = get_tangent_part_position(int_<I>(), dom);
		BOOST_ASSERT(pos < dimension(dom));
		const unsigned int dim = dimension(std::get<I>(dom));
		BOOST_ASSERT(dim == p.size());
		BOOST_ASSERT(t.size() == dimension(dom));
//		constexpr int ts = tangent_size<tuple<Args...>>::value;
//		if(ts == -1) {
			t.segment(pos, dim) = p;
//		}
//		else {
//			t.template segment<ts>(pos) = p;
//		}
	}

	// vecX<K> concatenate(const std::tuple<vecX<K>>& parts, unsigned int dim) {
	// 	unsigned int p = 0;
	// 	vecX<K> result(dim);
	// 	for(const vecX<K>& u : parts) {
	// 		result.segment(p, u.size()) = u;
	// 		p += u.size();
	// 	}
	// 	return result;
	// }
}

namespace detail
{
	template<unsigned int I>
	struct exp_helper {
		template<typename T, typename... Args>
		static inline void apply(
			T,
			typename state_type<tuple<Args...>>::type* result,
			const tuple<Args...>& dom,
			const typename state_type<tuple<Args...>>::type& x,
			const typename tangent_type<T,tuple<Args...>>::type& y) 
		{
			std::get<I>(*result) = domains::exp<T>(
				std::get<I>(dom),
				std::get<I>(x),
				get_tangent_part<I,T>(dom, y));
		}
	};
}

template<typename T, typename... Args>
typename state_type<tuple<Args...>>::type exp(
	const tuple<Args...>& dom,
	const typename state_type<tuple<Args...>>::type& x,
	const typename tangent_type<T,tuple<Args...>>::type& y
) {
	typename state_type<tuple<Args...>>::type result;
	detail::for_each<
			std::tuple_size<tuple<Args...>>::value,
			detail::exp_helper
	>(T(), &result, dom, x, y);
	return result;
}

namespace detail
{
	template<unsigned int I>
	struct log_helper {
		template<typename T, typename... Args>
		static inline void apply(
			T,
			typename tangent_type<T,tuple<Args...>>::type* result,
			const tuple<Args...>& dom,
			const typename state_type<tuple<Args...>>::type& x,
			const typename state_type<tuple<Args...>>::type& y) 
		{
			set_tangent_part<I,T>(dom, *result, domains::log<T>(
				std::get<I>(dom),
				std::get<I>(x),
				std::get<I>(y)));
		}
	};
}

template<typename T, typename... Args>
typename tangent_type<T,tuple<Args...>>::type log(
	const tuple<Args...>& dom,
	const typename state_type<tuple<Args...>>::type& x,
	const typename state_type<tuple<Args...>>::type& y
) {
	typename tangent_type<T,tuple<Args...>>::type result(dimension(dom));
	detail::for_each<
			std::tuple_size<tuple<Args...>>::value,
			detail::log_helper
	>(T(), &result, dom, x, y);
	return result;
}

namespace detail
{
	template<unsigned int I>
	struct random_helper {
		template<typename... Args>
		static inline void apply(
			typename state_type<tuple<Args...>>::type* result,
			const tuple<Args...>& dom) 
		{
			std::get<I>(*result) = random(std::get<I>(dom));
		}
	};

}

template<typename... Args>
typename state_type<tuple<Args...>>::type random(const tuple<Args...>& dom) {
	typename state_type<tuple<Args...>>::type result;
	detail::for_each<std::tuple_size<tuple<Args...>>::value, detail::random_helper>(&result, dom);
	return result;
}

namespace detail
{
	template<unsigned int I>
	struct random_neighbour_helper {
		template<typename... Args>
		static inline void apply(
			typename state_type<tuple<Args...>>::type* result,
			const tuple<Args...>& dom,
			const typename state_type<tuple<Args...>>::type& x,
			double radius) 
		{
			std::get<I>(*result) = random_neighbour(std::get<I>(dom), std::get<I>(x), radius);
		}
	};

}

template<typename... Args>
typename state_type<tuple<Args...>>::type random_neighbour(const tuple<Args...>& dom, const typename state_type<tuple<Args...>>::type& x, double radius) {
	typename state_type<tuple<Args...>>::type result;
	detail::for_each<std::tuple_size<tuple<Args...>>::value, detail::random_neighbour_helper>(&result, dom, x, radius);
	return result;
}

namespace detail
{
	template<unsigned int I>
	struct mean_helper {
		template<typename W, typename... Args>
		static inline void apply(
			typename state_type<tuple<Args...>>::type* result,
			const tuple<Args...>& dom,
			const std::vector<W>& weights,
			const std::vector<typename state_type<tuple<Args...>>::type>& states) 
		{
			std::vector<typename std::tuple_element<I,typename state_type<tuple<Args...>>::type>::type> parts(states.size());
			for(std::size_t i=0; i<states.size(); i++) {
				parts[i] = std::get<I>(states[i]);
			}
			std::get<I>(*result) = mean(std::get<I>(dom), weights, parts);
		}
	};

}

template<typename W, typename... Args>
typename state_type<tuple<Args...>>::type mean(const tuple<Args...>& dom, const std::vector<W>& weights, const std::vector<typename state_type<tuple<Args...>>::type>& states) {
	typename state_type<tuple<Args...>>::type result;
	detail::for_each<std::tuple_size<tuple<Args...>>::value, detail::mean_helper>(&result, dom, weights, states);
	return result;
}

namespace detail
{
	template<unsigned int I>
	struct lerp_helper {
		template<typename W, typename... Args>
		static inline void apply(
			typename state_type<tuple<Args...>>::type* result,
			const tuple<Args...>& dom,
			W p,
			const typename state_type<tuple<Args...>>::type& a,
			const typename state_type<tuple<Args...>>::type& b) 
		{
			std::get<I>(*result) = lerp(std::get<I>(dom), p, std::get<I>(a), std::get<I>(b));
		}
	};

}

template<typename W, typename... Args>
typename state_type<tuple<Args...>>::type lerp(const tuple<Args...>& dom, W p, const typename state_type<tuple<Args...>>::type& a, const typename state_type<tuple<Args...>>::type& b) {
	typename state_type<tuple<Args...>>::type result;
	detail::for_each<std::tuple_size<tuple<Args...>>::value, detail::lerp_helper>(&result, dom, p, a, b);
	return result;
}

}}
//---------------------------------------------------------------------------
#endif
