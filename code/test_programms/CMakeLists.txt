cmake_minimum_required(VERSION 2.8)
project(ft_robot_simulation_tests)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_WARNINGS_PRIV} -g")

add_subdirectory(network)

set(SOURCE_FILES_MOVE movement.cxx)
add_executable(move ${SOURCE_FILES_MOVE})
target_link_libraries(move pthread rpc network world algo)

set(SOURCE_FILES_MOVE fuel_test.cxx)
add_executable(fuel_test ${SOURCE_FILES_MOVE})
target_link_libraries(fuel_test pthread rpc network world)

set(SOURCE_FILES_IMPULS impuls.cxx)
add_executable(impuls ${SOURCE_FILES_IMPULS})

set(SOURCE_FILES_MSGPACK msgpack.c)
add_executable(genmsgpack ${SOURCE_FILES_MSGPACK})
target_link_libraries(genmsgpack msgpackc)

