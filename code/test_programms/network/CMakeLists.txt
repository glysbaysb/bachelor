cmake_minimum_required(VERSION 2.8)
project(network_test)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_WARNINGS}")

set(SOURCE_FILES_NETWORK main.c)
add_executable(network ${SOURCE_FILES_NETWORK})
target_link_libraries(network
)

set(SOURCE_FILES_BCH bch4836.c)
add_executable(bch ${SOURCE_FILES_BCH})
target_link_libraries(bch
)

set(SOURCE_FILES_RS rs.c)
add_executable(rs ${SOURCE_FILES_RS})
target_link_libraries(rs
)

set(SOURCE_FILES_LIQUID liquid.c)
add_executable(liquid_ ${SOURCE_FILES_LIQUID})
target_link_libraries(liquid_
	liquid
)

add_subdirectory(rscode-1.3)


