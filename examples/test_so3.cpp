#include <q0/q0.hpp>
#include <q0/algorithms.hpp>
#include <q0/domains/so3.hpp>
#include <q0/algorithms/apso.hpp>
#include <random>
#include <iostream>

std::default_random_engine rnde;
std::uniform_real_distribution<float> unif(-1.0f,+1.0f);

unsigned int f_eval_count = 0;

std::vector<std::pair<Eigen::Vector3f,Eigen::Vector3f>> data;

float f(const Eigen::Quaternion<float>& u) {
	f_eval_count ++;
	Eigen::Matrix3f mat = u.matrix();
	float rmse = 0.0f;
	for(std::size_t i=0; i<data.size(); i++) {
		rmse += (data[i].second - mat*data[i].first).squaredNorm();
	}
	return rmse;
}

bool stop_condition(const Eigen::Quaternion<float>& u, float s) {
//	std::cout << "{" << u << "} -> " << s << std::endl;
	return f_eval_count >= 1000 || s < 0.001f;
}

int main(int argc, char** argv)
{
	q0::domains::so3<float> dom;

	Eigen::Quaternion<float> expected = Eigen::Quaternion<float>{ unif(rnde), unif(rnde), unif(rnde), unif(rnde)}.normalized();
	Eigen::Matrix3f expected_mat = expected.matrix();

	for(int i=0; i<30; i++) {
		Eigen::Vector3f v{ unif(rnde), unif(rnde), unif(rnde) };
		data.push_back({ v, expected_mat*v });
	}

	std::cout << "Expected: " << q0::print(dom,expected) << " -> " << f(expected) << std::endl;

	q0::algorithms::apso alg;
	auto p = q0::minimize(dom, f, alg, q0::stop_condition(&stop_condition));

	std::cout << "Actual:   " << q0::print(dom,p.state) << " -> " << p.score << std::endl;
	std::cout << "Number of evaluations: " << f_eval_count << std::endl;
}
