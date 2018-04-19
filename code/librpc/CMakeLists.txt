cmake_minimum_required(VERSION 2.8)
project(rpc)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_WARNINGS_PRIV}")
link_libraries("-static")

set(SOURCE_FILES_LIBRPC rpc.c)
add_library(${PROJECT_NAME} STATIC
	${SOURCE_FILES_LIBRPC}
)

find_library(MSGPACK_STATIC NAMES libmsgpackc.a) 
target_link_libraries(${PROJECT_NAME} ${MSGPACK_STATIC})
target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_DIRECTORY}
)

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-static")

