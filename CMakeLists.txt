cmake_minimum_required(VERSION 2.6)

add_definitions(-std=c++0x -DBOOST_DISABLE_ASSERTS)

include_directories(
	/home/david/git/DanvilTools/CT/src
)

project(q0)
add_subdirectory(examples)
