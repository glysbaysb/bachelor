cmake_minimum_required(VERSION 2.8)
project(ft_robot_simulation_tests)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_WARNINGS_PRIV}")

add_subdirectory(rpc)
add_subdirectory(ecc)
#add_subdirectory(simulation)
add_subdirectory(network)
add_subdirectory(algo)
add_subdirectory(ficfg)
