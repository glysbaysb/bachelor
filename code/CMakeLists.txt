cmake_minimum_required(VERSION 2.8)
project(ft_robot_simulation)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -D_POSIX_C_SOURCE=200112L -g")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -D_POSIX_C_SOURCE=200112L -g")
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG")
set(CMAKE_C_FLAGS_DEBUG "-DDEBUG")

set(CXX_WARNINGS "-Wall -Wextra -Wpedantic -Wformat=2 -Wmissing-include-dirs \
-Wswitch-default -Wuninitialized -Wfloat-equal -Wundef -Wshadow -Wcast-qual \
-Wcast-align -Wconversion -Wlogical-op \
-Wmissing-declarations -Wredundant-decls -Winline -Wdisabled-optimization \
-Wnarrowing ")
set(C_WARNINGS "-Wall -Wextra -Wpedantic -Wformat=2 -Wmissing-include-dirs \
-Wswitch-default -Wuninitialized -Wfloat-equal -Wundef -Wshadow -Wcast-qual \
-Wcast-align -Wconversion -Wjump-misses-init -Wlogical-op -Wmissing-prototypes \
-Wmissing-declarations -Wredundant-decls -Winline -Wdisabled-optimization \
-Wnarrowing ")


include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(tests)
add_subdirectory(voter)

set(SOURCE_FILES_MSGPACK msgpack.c)
add_executable(genmsgpack ${SOURCE_FILES_MSGPACK})
target_link_libraries(genmsgpack msgpackc)

