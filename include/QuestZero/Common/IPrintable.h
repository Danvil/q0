/*
 * IPrintable.h
 *
 *  Created on: Jul 31, 2010
 *      Author: david
 */

#ifndef Q0_COMMON_IPRINTABLE_H_
#define Q0_COMMON_IPRINTABLE_H_
//---------------------------------------------------------------------------
//#include <boost/shared_ptr.hpp>
#include <string>
#include <sstream>
#include <ostream>
//---------------------------------------------------------------------------
namespace Q0 {
//---------------------------------------------------------------------------

/** Interface for printable classes */
class IPrintable
{
public:
	virtual ~IPrintable() {}

	/** Virtual functions which prints the object to a stream */
	virtual void print(std::ostream& os) const = 0;

	/** Calls print and writes to a string which is returned */
	std::string toString() const {
		std::stringstream ss;
		print(ss);
		return ss.str();
	}

};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------

inline
std::ostream& operator<<(std::ostream& os, const Q0::IPrintable& p) {
	p.print(os);
	return os;
}

//inline
//std::ostream& operator<<(std::ostream& os, Q0::IPrintable* p) {
//	p->print(os);
//	return os;
//}
//
//template<class T>
//std::ostream& operator<<(std::ostream& os, const boost::shared_ptr<T>& p) {
//	boost::shared_ptr<Q0::IPrintable> p_printable(p, boost::detail::dynamic_cast_tag());
//	if(p_printable) {
//		p_printable->print(os);
//	} else {
//		os << p.get();
//	}
//	return os;
//}

//---------------------------------------------------------------------------
#endif
