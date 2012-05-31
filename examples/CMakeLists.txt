include_directories(
	${q0_SOURCE_DIR}/include
	${OpenCV_INCLUDE_DIRS}
)

add_executable(Angular optimization/Angular.cpp)
#add_executable(PointCloudRegistration_Rot optimization/PointCloudRegistration_Rot.cpp)
#add_executable(PointCloudRegistration_LocRot optimization/PointCloudRegistration_LocRot.cpp)
#add_executable(PointCloudRegistration_Rot_Multi optimization/PointCloudRegistration_Rot_Multi.cpp)
#add_executable(PointCloudRegistration_LocRot_Multi optimization/PointCloudRegistration_LocRot_Multi.cpp)
#add_executable(Problem07 optimization/Problem07.cpp)
add_executable(SpecialFunctions_3 optimization/SpecialFunctions_3.cpp)
add_executable(SpecialFunctions_31 optimization/SpecialFunctions_31.cpp)
#add_executable(Problem09 tracking/Problem09.cpp)
#add_executable(Problem10 tracking/Problem10.cpp)

#target_link_libraries(examples
#	${OpenCV_LIBS}
#	boost_program_options
#)

