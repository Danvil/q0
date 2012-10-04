#include_directories(
#	${q0_SOURCE_DIR}/include
#	${OpenCV_INCLUDE_DIRS}
#)

add_executable(test_cartesian test_cartesian.cpp)
add_executable(test_cartesian_box test_cartesian_box.cpp)
add_executable(test_so2 test_so2.cpp)
add_executable(test_trace test_trace.cpp)
