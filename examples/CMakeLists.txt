#include_directories(
#	${q0_SOURCE_DIR}/include
#	${OpenCV_INCLUDE_DIRS}
#)

add_executable(test_cartesian test_cartesian.cpp)
target_link_libraries(test_cartesian boost_program_options)

add_executable(test_so2 test_so2.cpp)
target_link_libraries(test_so2 boost_program_options)
