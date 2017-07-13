cmake_minimum_required(VERSION 2.8)
project(rpc_tests)

set(SOURCE_FILES_RPC test_rpc.cxx)
add_executable(rpc ${SOURCE_FILES_RPC})
target_link_libraries(rpc
	gtest
	gmock
	pthread
)
