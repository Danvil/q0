include_directories(
	${q0_SOURCE_DIR}/include
	${OpenCV_INCLUDE_DIRS}
)

add_executable(SpecialFunctions_3 optimization/SpecialFunctions_3.cpp)
add_executable(SpecialFunctions_31 optimization/SpecialFunctions_31.cpp)
add_executable(PointCloudRegistration_Rot optimization/PointCloudRegistration_Rot.cpp)
add_executable(PointCloudRegistration_LocRot optimization/PointCloudRegistration_LocRot.cpp)

#target_link_libraries(examples
#	${OpenCV_LIBS}
#	boost_program_options
#)

