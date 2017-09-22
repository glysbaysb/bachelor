cmake_minimum_required(VERSION 2.8)
project(network_test)

set(SOURCE_FILES_NETWORK test_network.cxx)
add_executable(test_network ${SOURCE_FILES_NETWORK})
target_link_libraries(test_network
	gtest
	gmock
	pthread
	network
)
