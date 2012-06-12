/*
 * StateScoreVector.cpp
 *
 *  Created on: Jun 7, 2012
 *      Author: david
 */

#include <QuestZero/Common/StateScoreVector.h>
#include <iostream>

int main()
{
	struct state_t { int x; float y; };
	struct score_t { double q; bool v; };

	Q0::StateScoreVector<state_t, score_t> list;

	auto id = Q0::add_sample(list);
	Q0::set_state(list, id, {0,0.5f});
	Q0::set_score(list, id, {0.123,false});
	std::cout << Q0::get_state(list, id).y << std::endl;
	std::cout << Q0::get_score(list, id).q << std::endl;

	Q0::add_samples(list, 13);

	std::cout << Q0::num_samples(list) << std::endl;

}

