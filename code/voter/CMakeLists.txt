cmake_minimum_required(VERSION 2.8)
project(voter)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_WARNINGS_PRIV}")

set(SOURCE_FILES_VOTER main.cxx)
add_executable(voter ${SOURCE_FILES_VOTER})
target_link_libraries(voter world network)
