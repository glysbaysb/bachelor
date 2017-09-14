cmake_minimum_required(VERSION 2.8)
project(rs_test)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_WARNINGS}")

set(SOURCE_FILES_rs_ example.c rs.c galois.c berlekamp.c crcgen.c)
add_executable(rs_ ${SOURCE_FILES_rs_})
target_link_libraries(rs_
)
