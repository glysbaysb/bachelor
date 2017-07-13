cmake_minimum_required(VERSION 2.8)
project(libworld)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_WARNINGS}")

set(SOURCE_FILES_LIBWORLD libworld.c)
add_library(libworld STATIC
	${SOURCE_FILES_LIBWORLD}
)
target_link_libraries(libworld nanomsg pthread librpc)
target_include_directories(libworld PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)


