cmake_minimum_required(VERSION 2.8)
project(libalgo)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_WARNINGS_PRIV}")

# Declare the library
set(SOURCE_FILES_LIBALGO
	algo.cxx algo.h)
add_library(algo STATIC
	${SOURCE_FILES_LIBALGO}
)
target_link_libraries(algo)
# Specify the include directories exported by this library
target_include_directories(algo PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

