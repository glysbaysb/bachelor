cmake_minimum_required(VERSION 2.8)
project(network_test)

set(SOURCE_FILES_NETWORK main.c)
add_executable(test_NETWORK ${SOURCE_FILES_NETWORK})
target_link_libraries(test_NETWORK
)
