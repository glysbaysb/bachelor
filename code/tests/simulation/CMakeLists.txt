cmake_minimum_required(VERSION 2.8)
project(simulation_tests)

set(SOURCE_FILES_WORLD test_world.cxx)
add_executable(test_world ${SOURCE_FILES_WORLD})
target_link_libraries(test_world
    gtest
    gmock
	pthread
	sim
)

set(SOURCE_FILES_ROBOT test_robot.cxx)
add_executable(test_robot ${SOURCE_FILES_ROBOT})
target_link_libraries(test_robot
    gtest
    gmock
	pthread
	sim
)

#install(TARGETS testfoo DESTINATION bin)
