cmake_minimum_required(VERSION 2.8)
project(voter)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_WARNINGS}")

set(SOURCE_FILES_VOTER main.c)
add_executable(voter ${SOURCE_FILES_VOTER})
target_link_libraries(voter libworld)
