cmake_minimum_required(VERSION 2.8)
project(ecc_tests)

set(SOURCE_FILES_ecc test_ecc.cxx)
add_executable(test_ecc ${SOURCE_FILES_ecc})
target_link_libraries(test_ecc
	gtest
	gmock
	pthread
	libecc
)
