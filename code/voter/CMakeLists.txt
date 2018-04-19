cmake_minimum_required(VERSION 2.8)
project(voter)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_WARNINGS_PRIV}")

set(SOURCE_FILES_VOTER main.cxx)
add_executable(${PROJECT_NAME} ${SOURCE_FILES_VOTER})
target_link_libraries(${PROJECT_NAME} world network)
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-static")

