/*
 * Cone.cpp
 *
 *  Created on: Jun 1, 2012
 *      Author: david
 */

#include <QuestZero/Spaces/Cartesian.h>
#include <Eigen/Dense>

int main(int argc, char** argv)
{
	Q0::Spaces::InfiniteCartesianSpace<Eigen::Vector3f> space1;

	Q0::Spaces::InfiniteCartesianSpace<Eigen::Matrix<float,31,1> > space2;

	return 1;
}

