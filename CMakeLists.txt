cmake_minimum_required(VERSION 2.6)

set(EIGEN3_INCLUDE_DIR "/usr/local/include/eigen3" CACHE STRING "Eigen3 include directory")

add_definitions(
	-std=c++0x
	-DBOOST_DISABLE_ASSERTS
	-Wall
	-Werror
)

include_directories(
	${q0_SOURCE_DIR}/include
	${EIGEN3_INCLUDE_DIR}
)

project(q0)
add_subdirectory(examples)
add_subdirectory(test)
