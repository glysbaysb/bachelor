cmake_minimum_required(VERSION 2.8)
project(simulation_tests)

set(SOURCE_FILES_WORLD test_world.cxx)
add_executable(world ${SOURCE_FILES_WORLD})
target_link_libraries(world
    gtest
    gmock
	pthread
	libsim
)

set(SOURCE_FILES_ROBOT test_robot.cxx)
add_executable(robot ${SOURCE_FILES_ROBOT})
target_link_libraries(robot
    gtest
    gmock
	pthread
	libsim
)

#install(TARGETS testfoo DESTINATION bin)
