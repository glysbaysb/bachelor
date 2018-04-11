cmake_minimum_required(VERSION 2.8)
project(algo_test)

set(SOURCE_FILES_ALGO test_algo.cxx)
add_executable(test_algo ${SOURCE_FILES_ALGO})
target_link_libraries(test_algo
    gtest
    gmock
	pthread
	algo
)

set(SOURCE_FILES_VECTOR test_vector.cxx)
add_executable(test_vector ${SOURCE_FILES_VECTOR})
target_link_libraries(test_vector
	gtest
	gmock
	pthread
	algo
)
