cmake_minimum_required(VERSION 2.8)
project(rs2)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_WARNINGS}")

set(SRC_LIBECC rs.c galois.c berlekamp.c crcgen.c)
add_library(libecc STATIC
	${SRC_LIBECC}
)
target_link_libraries(libecc)
target_include_directories(libecc PUBLIC
	${CMAKE_CURRENT_DIRECTORY}
)

set(SOURCE_FILES_EXAMPLE example.c)
add_executable(rs2 ${SOURCE_FILES_EXAMPLE})
target_link_libraries(rs2
	libecc
)


