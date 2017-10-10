#
#
#
cmake_minimum_required(VERSION 2.8)
project(simulation)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_WARNINGS_PRIV}")

# Declare the library
set(SOURCE_FILES_LIB
	world.cxx world.h
	robot.cxx robot.h)
add_library(sim STATIC
    ${SOURCE_FILES_LIB}
)
target_link_libraries(sim)
# Specify the include directories exported by this library
target_include_directories(sim PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# and the actual program
set(SOURCE_FILES_SIMULATION main.cxx)
add_executable(simulation ${SOURCE_FILES_SIMULATION})
target_link_libraries(simulation m sim)
