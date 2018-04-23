#
#
#
cmake_minimum_required(VERSION 2.8)
project(controller)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_WARNINGS_PRIV}")

# and the actual program
set(SOURCE_FILES_CONTROLLER main.cxx)
add_executable(controller ${SOURCE_FILES_CONTROLLER})
target_link_libraries(controller m network rpc algo)

#set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-static")
