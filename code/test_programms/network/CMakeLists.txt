cmake_minimum_required(VERSION 2.8)
project(network_test)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_WARNINGS}")

set(SOURCE_FILES_NETWORK main.c)
add_executable(network ${SOURCE_FILES_NETWORK})
target_link_libraries(network
)

set(SOURCE_FILES_LIQUID liquid.c)
add_executable(liquid_ ${SOURCE_FILES_LIQUID})
target_link_libraries(liquid_
	liquid
)

set(SOURCE_FILES_VITERBI viterbi.cpp)
add_executable(viterbi ${SOURCE_FILES_VITERBI})
target_link_libraries(viterbi
)

add_subdirectory(rscode-1.3)
