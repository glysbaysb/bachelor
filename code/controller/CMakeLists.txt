#
#
#
cmake_minimum_required(VERSION 2.8)
project(simulation)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_WARNINGS}")

# and the actual program
set(SOURCE_FILES_CONTROLLER main.cxx)
add_executable(controller ${SOURCE_FILES_NETWORK})
target_link_libraries(controller m network)
