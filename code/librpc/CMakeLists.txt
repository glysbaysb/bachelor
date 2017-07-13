cmake_minimum_required(VERSION 2.8)
project(librpc)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_WARNINGS}")

set(SOURCE_FILES_LIBRPC rpc.c)
add_library(librpc STATIC
	${SOURCE_FILES_LIBRPC}
)
target_link_libraries(librpc msgpackc)
target_include_directories(librpc PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
