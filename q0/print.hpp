#ifndef Q0_PRINT_H_
#define Q0_PRINT_H_
#include <q0/domains.hpp>
#include <iostream>
#include <sstream>
//---------------------------------------------------------------------------
namespace q0 {

namespace detail
{
	template<typename Domain>
	struct serialize_impl
	{
		serialize_impl(const Domain& domain, const typename domains::state_type<Domain>::type& x)
		: domain_(domain), x_(x) {}

		void stream(std::ostream& os) const {
			print(os, domain_, x_);
		}

		std::string str() const {
			std::stringstream ss;
			stream(ss);
			return ss.str();
		}

		friend std::ostream& operator<<(std::ostream& os, const serialize_impl<Domain>& s) {
			s.stream(os);
			return os;
		}

	private:
		const Domain& domain_;
		const typename domains::state_type<Domain>::type& x_;
	};

}

template<typename Domain>
detail::serialize_impl<Domain> print(const Domain& dom, const typename domains::state_type<Domain>::type& x)
{
	return detail::serialize_impl<Domain>(dom, x);
}

}
//---------------------------------------------------------------------------
#endif
