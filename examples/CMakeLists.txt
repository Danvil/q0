#include_directories(
#	${q0_SOURCE_DIR}/include
#	${OpenCV_INCLUDE_DIRS}
#)

add_executable(test_cartesian test_cartesian.cpp)
add_executable(test_cartesian_box test_cartesian_box.cpp)
add_executable(test_so2 test_so2.cpp)
add_executable(test_trace test_trace.cpp)
add_executable(test_tuple test_tuple.cpp)
add_executable(test_vector test_vector.cpp)
add_executable(test_algorithms test_algorithms.cpp)
add_executable(test_algorithms_domains test_algorithms_domains.cpp)
add_executable(test_domains test_domains.cpp)
add_executable(test_print test_print.cpp)

add_executable(arm arm.cpp)
target_link_libraries(arm boost_program_options)
