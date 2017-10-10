cmake_minimum_required(VERSION 2.8)
project(rpc)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_WARNINGS_PRIV}")

set(SOURCE_FILES_LIBRPC rpc.c)
add_library(${PROJECT_NAME} STATIC
	${SOURCE_FILES_LIBRPC}
)
target_link_libraries(${PROJECT_NAME} msgpackc)
target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_DIRECTORY}
)
