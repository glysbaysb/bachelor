cmake_minimum_required(VERSION 2.8)
project(ecc)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_WARNINGS_PRIV}")

set(SRC_LIBECC_FILES rs.c galois.c berlekamp.c crcgen.c ecc.h)
add_library(${PROJECT_NAME} STATIC
	${SRC_LIBECC_FILES}
)
target_link_libraries(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_DIRECTORY}
)
