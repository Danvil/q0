#include_directories(
#	${q0_SOURCE_DIR}/include
#	${OpenCV_INCLUDE_DIRS}
#)

add_executable(test1 test1.cpp)
target_link_libraries(test1 boost_program_options)
