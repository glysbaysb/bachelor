#
#
#
cmake_minimum_required(VERSION 2.8)
project(voter)

# and the actual program
set(SOURCE_FILES_VOTER main.c)
add_executable(voter ${SOURCE_FILES_VOTER})
target_link_libraries(voter nanomsg msgpackc)
