cmake_minimum_required(VERSION 2.8)
project(network_test)

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
