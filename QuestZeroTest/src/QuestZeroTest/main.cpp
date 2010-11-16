/*
 * main.cpp
 *
 *  Created on: Sep 4, 2010
 *      Author: david
 */

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

//---------------------------------------------------------------------------

int main(int argc, char* argv[]) {
	cout << "Hello QuestZero!" << endl;
	cout << endl;

	Problem01::run();
	Problem02::run();
	Problem03::run();
	Problem04::run();
	Problem05::run();
	Problem06::run();
	Problem07::run();
	Problem08::run();
	Problem09::run();

	cout << "Bye QuestZero!" << endl;
	return 0;
}

//---------------------------------------------------------------------------

#include "QuestZero/Common/RandomNumbers.cpp"

//---------------------------------------------------------------------------
