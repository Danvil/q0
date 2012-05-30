include_directories(
	${q0_SOURCE_DIR}/include
	${OpenCV_INCLUDE_DIRS}
)

add_executable(examples main.cpp Problem01.cpp)

target_link_libraries(examples
	${OpenCV_LIBS}
	boost_program_options
)

