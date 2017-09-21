cmake_minimum_required(VERSION 2.8)
project(network_test)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_WARNINGS}")

set(SOURCE_FILES_EXAMPLE example.c)
add_executable(rs2 ${SOURCE_FILES_EXAMPLE})
target_link_libraries(rs2
	ecc
)
