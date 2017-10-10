cmake_minimum_required(VERSION 2.8)
project(libnetwork)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_WARNINGS_PRIV}")

# Declare the library
set(SOURCE_FILES_LIBNETWORK
	network.cxx network.h)
add_library(network STATIC
	${SOURCE_FILES_LIBNETWORK}
)
target_link_libraries(network ecc)
# Specify the include directories exported by this library
target_include_directories(network PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
