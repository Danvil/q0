/*
 * Problem10.cpp
 *
 *  Created on: Mar 10, 2011
 *      Author: david
 */

#include <QuestZero/Spaces/Angular.h>
#include <Danvil/Tools/Log.h>
#include <Danvil/Tools/MoreMath.h>
#include <Danvil/String.h>
#include <stdexcept>
using namespace std;
//----------------------------------------------------------------------------//
namespace Problem10 {
//----------------------------------------------------------------------------//

typedef float Scalar;
typedef Q0::Spaces::Angular::IntervalAngularSpace<Scalar> AS;

#define TEST(COND, MSG) if(!(COND)) { LOG_ERROR << MSG; }

void TestContains(const AS& space, size_t n)
{
	TEST(space.contains(space.lower()), "Problem10: Angular space must contain lower")
	TEST(space.contains(space.upper()), "Problem10: Angular space must contain upper")
	if(space.lower() < space.upper()) {
		for(size_t i=0; i<n; i++) {
			Scalar s = space.lower() + Scalar(i) / Scalar(n) * (space.upper() - space.lower());
			TEST(space.contains(s), "Problem10: Angular space does not contain valid state " << s)
		}
	} else {
		for(size_t i=0; i<n; i++) {
			Scalar s = Scalar(i) / Scalar(n) * space.upper();
			TEST(space.contains(s), "Problem10: Angular space does not contain valid state " << s)
		}
		for(size_t i=0; i<n; i++) {
			Scalar s = space.lower() + Scalar(i) / Scalar(n) * (Danvil::TwoPi<Scalar>() - space.lower());
			TEST(space.contains(s), "Problem10: Angular space does not contain valid state " << s)
		}
	}
}

void TestRandom(const AS& space, size_t n, const char* str="")
{
	vector<Scalar> v(n);
	for(size_t i=0; i<n; i++) {
		Scalar s = space.random();
		v[i] = s;
	}
	Danvil::ctString::TextFile::WriteLines("/tmp/q0_test_angular_rnd" + std::string(str) + ".txt", v);
	for(size_t i=0; i<n; i++) {
		Scalar s = v[i];
		TEST(space.contains(s), "Problem10: Angular uniform sampling created invalid state " << s)
	}
}

void TestRandomCentered(const AS& space, size_t n, Scalar center, Scalar window, const char* str="")
{
	vector<Scalar> v(n);
	for(size_t i=0; i<n; i++) {
		Scalar s = space.random(center, window);
		v[i] = s;
	}
	Danvil::ctString::TextFile::WriteLines("/tmp/q0_test_angular_rndcenter" + std::string(str) + ".txt", v);
	for(size_t i=0; i<n; i++) {
		Scalar s = v[i];
		TEST(space.contains(s), "Problem10: Angular centered sampling created invalid state " << s)
	}
}

#define RAD(X) Danvil::MoreMath::Degree2Radians(Scalar(X))

void run()
{
	AS space;
	space.set_lower(Scalar(0));
	space.set_upper(Danvil::MoreMath::Degree2Radians(Scalar(90.0)));

	unsigned int dim = space.dimension();
	TEST(dim == 1, "Problem10: Angular dimension must be 1 not " << dim)

	TestContains(space, 100);
	TestRandom(space, 1000);
	TestRandomCentered(space, 100, RAD(0), RAD(10.0), "01");
	TestRandomCentered(space, 100, RAD(0), RAD(100.0), "02");
	TestRandomCentered(space, 100, RAD(90), RAD(10.0), "03");
	TestRandomCentered(space, 100, RAD(90), RAD(100.0), "04");
	TestRandomCentered(space, 100, RAD(45), RAD(10.0), "05");
	TestRandomCentered(space, 100, RAD(45), RAD(100.0), "06");
	TestRandomCentered(space, 100, RAD(170), RAD(10.0), "07");
	TestRandomCentered(space, 100, RAD(170), RAD(100.0), "08");


}

//----------------------------------------------------------------------------//
}
//----------------------------------------------------------------------------//
