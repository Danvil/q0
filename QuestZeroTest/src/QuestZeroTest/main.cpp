/*
 * main.cpp
 *
 *  Created on: Sep 4, 2010
 *      Author: david
 */

#include <Danvil/Tools/Log.h>
#include <iostream>
using std::cout;
using std::endl;
//---------------------------------------------------------------------------

namespace Problem01 { void run(); }
namespace Problem02 { void run(); }
namespace Problem03 { void run(); }
namespace Problem04 { void run(); }
namespace Problem05 { void run(); }
namespace Problem06 { void run(); }
namespace Problem07 { void run(); }
namespace Problem08 { void run(); }
namespace Problem09 { void run(); }
namespace Problem10 { void run(); }

#define RUN(n) LOG_NOTICE << ""; LOG_NOTICE << "Running Problem " << #n; Problem##n::run();

//---------------------------------------------------------------------------

int main(int argc, char* argv[]) {
	LOG_NOTICE << "Hello QuestZero!";

	RUN(01)
	RUN(02)
	RUN(03)
	//RUN(04)
	RUN(05)
	RUN(06)
	RUN(07)
	RUN(08)
	//RUN(09)
	RUN(10)

	LOG_NOTICE << "Bye QuestZero!";
	return 0;
}

//---------------------------------------------------------------------------
