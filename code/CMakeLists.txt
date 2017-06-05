cmake_minimum_required(VERSION 2.8)
project(ft_robot_simulation)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -D_POSIX_C_SOURCE=200112L -g")
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG")

set(CXX_WARNINGS "-Wall -Wextra -Wpedantic -Wformat=2 -Wmissing-include-dirs
-Wswitch-default -Wuninitialized -Wfloat-equal -Wundef -Wshadow -Wcast-qual
-Wcast-align -Wconversion -Wjump-misses-init -Wlogical-op -Wmissing-prototypes
-Wmissing-declarations -Wredundant-decls -Winline -Wdisabled-optimization
-Wnarrowing -Wstrict")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(simulation)
add_subdirectory(tests)