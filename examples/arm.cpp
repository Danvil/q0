/*
 * arm.cpp
 *
 *  Created on: Okt 02, 2012
 *      Author: david
 */

#include <q0/q0.hpp>
#include <q0/print.hpp>
#include <q0/algorithms.hpp>
#include <q0/domains/so2.hpp>
#include <q0/domains/tuple.hpp>
#include <q0/algorithms/random_search.hpp>
#include <q0/algorithms/apso.hpp>
#include <q0/algorithms/differential_evolution.hpp>
#include <Eigen/Dense>
#include <boost/program_options.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/algorithms/intersection.hpp>
#include <boost/geometry/algorithms/union.hpp>
#include <boost/geometry/geometries/adapted/c_array.hpp>
#include <boost/random.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real_distribution.hpp>
BOOST_GEOMETRY_REGISTER_C_ARRAY_CS(cs::cartesian)
#include <boost/format.hpp>
#include <iostream>
#include <fstream>

typedef q0::domains::tuple<q0::domains::so2<float>,q0::domains::so2<float>> domain_t;

typedef typename q0::domains::state_type<domain_t>::type state_t;

typedef float score_t;

typedef boost::geometry::model::d2::point_xy<float> pnt_t;

//typedef Eigen::Matrix<float, 2, 1> pnt_t;

//namespace boost { namespace geometry
//{
//namespace traits
//{
//	template<>
//	struct tag<pnt_t> {
//		typedef point_tag type;
//	};
//
//	template<>
//	struct coordinate_type<pnt_t> {
//		typedef float type;
//	};
//
//	template<>
//	struct coordinate_system<pnt_t> {
//		typedef cs::cartesian type;
//	};
//
//	template<>
//	struct dimension<pnt_t> : boost::mpl::int_<2> {};
//
//	template<std::size_t Dimension>
//	struct access<pnt_t,Dimension> {
//		static inline float get(const pnt_t& u) { return u[Dimension]; }
//		static inline void set(pnt_t& u, float x) { u[Dimension] = x; }
//	};
//}
//}}

typedef boost::geometry::model::polygon<pnt_t> polygon_t;

polygon_t create(const pnt_t& start, float angle, float length, float width) {
	static boost::random::mt19937 rng;
	boost::random::uniform_real_distribution<float> dist(-0.001f,+0.001f);

	float x = std::cos(angle);
	float y = std::sin(angle);
	std::vector<pnt_t> points = {
		{start.x() + 0.5f*width*y, start.y() - 0.5f*width*x},
		{start.x() - 0.5f*width*y, start.y() + 0.5f*width*x},
		{start.x() + length*x - 0.5f*width*y, start.y() + length*y + 0.5f*width*x},
		{start.x() + length*x + 0.5f*width*y, start.y() + length*y - 0.5f*width*x},
	};
	for(unsigned int i=0; i<points.size(); i++) {
		points[i].x(points[i].x() + dist(rng));
		points[i].y(points[i].y() + dist(rng));
	}
	points.push_back(points.front());
	polygon_t poly;
	boost::geometry::append(poly, points);
	return poly;
}

void printMathematica(std::ostream& os, const std::string& name, const polygon_t& poly) {
	os << name << "={";
	for(auto p : boost::geometry::exterior_ring(poly)) {
		os << "{" << p.x() << "," << p.y() << "}, ";
	}
	os << "}" << std::endl;
}

polygon_t arm(const state_t& u)
{
	constexpr float cLength = 3;
	constexpr float cWidth = 1;
	const float alpha = std::get<0>(u);
	const float beta = std::get<1>(u);

	const polygon_t p1 = create(
		{0,0},
		alpha,
		cLength, cWidth);

	polygon_t p2 = create(
		{cLength*0.95f*std::cos(alpha),cLength*0.95f*std::sin(alpha)},
		alpha+beta,
		cLength, cWidth);

	std::vector<polygon_t> output;
	boost::geometry::union_(p1, p2, output);

	// const polygon_t& result = output.front();

	// if(boost::geometry::area(result) < 0.01) {
	// 	// degenerated
	// 	for(auto& p : boost::geometry::exterior_ring(p2)) {
	// 		p.x(p.x() - 0.001);
	// 		p.y(p.y() - 0.001);
	// 	}
	// 	output.clear();
	// 	boost::geometry::union_(p1, p2, output);
	// }

	return output.front();
}

float compute_area_intesect(const polygon_t& a, const polygon_t& b)
{
	// intersect the polygons
	std::vector<polygon_t> output;
	boost::geometry::intersection(a, b, output);
	// compute area
	return std::accumulate(output.begin(), output.end(), 0.0f,
		[](float area, const polygon_t& x) {
			return area + boost::geometry::area(x);
		});
}

float compute_area_union(const polygon_t& a, const polygon_t& b)
{
	// intersect the polygons
	std::vector<polygon_t> output;
	boost::geometry::union_(a, b, output);
	// compute area
	return std::accumulate(output.begin(), output.end(), 0.0f,
		[](float area, const polygon_t& x) {
			return area + boost::geometry::area(x);
		});
}

float score(const polygon_t& a, const polygon_t& b)
{
	// compute intersect area
	float area_i = compute_area_intesect(a,b);
	// compute individual area
	float area_a = boost::geometry::area(a);
	float area_b = boost::geometry::area(b);
	// compute area of error area
	return area_a + area_b - 2.0f * area_i;
}

// float score(const polygon_t& a, const polygon_t& b)
// {
// 	float area_u = compute_area_union(a,b);
// 	float area_i = compute_area_intesect(a,b);
// 	return 1.0f - area_i / area_u;
// }

unsigned int f_eval_count = 0;

score_t arm_objective(const state_t& u)
{
	f_eval_count ++;
	const polygon_t ref = arm(state_t(q0::domains::angle<float>(-0.3f), q0::domains::angle<float>(0.5f)));
	const polygon_t cur = arm(u);
	return score(ref, cur);
}

bool stop_condition(const state_t& u, float s) {
	return f_eval_count >= 1000 || s < 0.001f;
}

void tracer(const q0::particle_vector<state_t,score_t>& particles, const q0::particle<state_t,score_t>& best) {
	static int iter = 0;
	static std::ofstream ofs;
	if(iter == 0) {
		ofs.open("trace_de.txt");
		ofs << "{";
	}
	else {
		ofs << ", {";
	}
	for(unsigned int i=0; i<particles.size(); i++) {
		if(i>0) ofs << ", ";
		float a, b;
		std::tie(a,b) = particles.states[i];
		a = q0::math::wrap(a, -boost::math::constants::pi<float>(), +boost::math::constants::pi<float>());
		b = q0::math::wrap(b, -boost::math::constants::pi<float>(), +boost::math::constants::pi<float>());
		ofs << "{" << a << ", " << b << "}";
//		std::cout << "{" << a << ", " << b << "} -> " << particles.scores[i] << std::endl;
	}
	ofs << "}";
	float a, b;
	std::tie(a,b) = best.state;
	std::cout << "Iteration " << iter << ": " << particles.size() << " particles, best = {" << a << "," << b << "} -> " << best.score << std::endl;
	iter++;
}

int main(int argc, char* argv[])
{
	unsigned int p_num_particles = 16384;
	bool p_print_result_state = false;
	std::string p_algo_name = "";
	std::string p_mode = "";
	{
		namespace po = boost::program_options;
		po::options_description desc("Allowed options");
		desc.add_options()
			("help", "produce help message")
			("num_particles", po::value<unsigned int>(&p_num_particles), "number of particles to use")
			("verbose", po::value<bool>(&p_print_result_state), "print result vectors")
			("algorithm", po::value<std::string>(&p_algo_name), "name of algorithm to use, empty string to run all")
			("mode", po::value<std::string>(&p_mode), "mode = normale/'', test, evaluation")
		;

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);
	}

	if(p_mode.empty() || p_mode == "optimize") {
		std::cout << "Optimizing in space [-pi|+pi]^2" << std::endl;
		domain_t dom;
		q0::control::TestAndTrace<state_t, score_t> control;
		control.tester = &stop_condition;
		control.tracer = &tracer;
		auto p = q0::minimize<q0::algorithms::differential_evolution>::apply(dom, &arm_objective, control);
		std::cout << "{" << std::get<0>(p.state) << "," << std::get<1>(p.state) << "} -> " << p.score << std::endl;
		std::cout << "Number of evaluations: " << f_eval_count << std::endl;
	}
	else if(p_mode == "test") {
		for(float x : {-0.5f, -0.3f, -0.1f, 0.0f}) {
			std::cout << x << " -> " << arm_objective(state_t(x,0.5f)) << std::endl;
		}
	}
	else if(p_mode == "evaluation") {
		constexpr unsigned int N = 99;
		const float R = -boost::math::constants::pi<float>();
		const float D = 2.0f * R / static_cast<float>(N);
		std::vector<std::vector<float>> scores(N, std::vector<float>(N));
		for(unsigned int i=0; i<N; i++) {
			for(unsigned int j=0; j<N; j++) {
				q0::domains::angle<float> x = -R + D*static_cast<float>(j);
				q0::domains::angle<float> y = -R + D*static_cast<float>(i);
				float s = arm_objective(state_t(x,y));
				scores[i][j] = s;
			}
		}
		// // write to file
		// std::ofstream ofs("f.csv");
		// for(unsigned int i=0; i<N; i++) {
		// 	for(unsigned int j=0; j<N; j++) {
		// 		ofs << scores[i][j];
		// 		if(j+1 != N) ofs << "\t";
		// 	}
		// 	ofs << std::endl;
		// }
		// write to file
		std::ofstream ofs("f.csv");
		for(unsigned int i=0; i<N; i++) {
			for(unsigned int j=0; j<N; j++) {
				float x = -R + D*static_cast<float>(j);
				float y = -R + D*static_cast<float>(i);
				ofs << x << "\t" << y << "\t" << scores[i][j] << std::endl;
			}
		}
	}
	else {
		std::cerr << "Unknown mode '" << p_mode << "'" << std::endl;
	}

	return 1;
}
