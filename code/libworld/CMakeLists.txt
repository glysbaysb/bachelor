cmake_minimum_required(VERSION 2.8)
project(world)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_WARNINGS_PRIV}")

set(SOURCE_FILES_LIBWORLD world.c)
add_library(${PROJECT_NAME} STATIC
	${SOURCE_FILES_LIBWORLD}
)

target_link_libraries(${PROJECT_NAME} ${CMAKE_BINARY_DIR}/third_party/nanomsg/libnanomsg.a pthread rpc ficfg anl)
target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_DIRECTORY}
)
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-static")

