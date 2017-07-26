cmake_minimum_required(VERSION 2.8)
project(rpc_tests)

set(SOURCE_FILES_RPC test_rpc.cxx)
add_executable(test_rpc ${SOURCE_FILES_RPC})
target_link_libraries(test_rpc
	gtest
	gmock
	pthread
	librpc
	libworld
)
