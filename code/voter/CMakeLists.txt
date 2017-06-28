cmake_minimum_required(VERSION 2.8)
project(voter)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_WARNINGS}")

# Declare the library
set(SOURCE_FILES_LIB libworld.c rpc.c)
add_library(libworld STATIC
    ${SOURCE_FILES_LIB}
)
target_link_libraries(libworld nanomsg msgpackc pthread)
# Specify the include directories exported by this library
target_include_directories(libworld PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# and the actual program
set(SOURCE_FILES_VOTER main.c)
add_executable(voter ${SOURCE_FILES_VOTER})
target_link_libraries(voter libworld)
