cmake_minimum_required(VERSION 2.8)
project(ft_robot_simulation)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Build everything statically
set(BUILD_SHARED_LIBRARIES OFF)
set(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
set(NN_STATIC_LIB on)

# Build third party libs
add_subdirectory(third_party/nanomsg)
include_directories(third_party/)
add_subdirectory(third_party/msgpack)
include_directories(third_party/msgpack/include)
add_subdirectory(third_party/googletest)

# Compile Flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -D_POSIX_C_SOURCE=200112L -g")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -D_POSIX_C_SOURCE=200112L -g")
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG")
set(CMAKE_C_FLAGS_DEBUG "-DDEBUG")

set(CXX_WARNINGS_PRIV "-Wall -Wextra -Wpedantic -Wformat=2 -Wmissing-include-dirs \
-Wswitch-default -Wuninitialized -Wfloat-equal -Wundef -Wshadow -Wcast-qual \
-Wcast-align -Wconversion -Wlogical-op \
-Wmissing-declarations -Wredundant-decls -Winline -Wdisabled-optimization \
-Wnarrowing ")
set(C_WARNINGS_PRIV "-Wall -Wextra -Wpedantic -Wformat=2 -Wmissing-include-dirs \
-Wswitch-default -Wuninitialized -Wfloat-equal -Wundef -Wshadow -Wcast-qual \
-Wcast-align -Wconversion -Wjump-misses-init -Wlogical-op -Wmissing-prototypes \
-Wmissing-declarations -Wredundant-decls -Winline -Wdisabled-optimization \
-Wnarrowing ")


# build my own stuff
add_subdirectory(librpc)
add_subdirectory(libworld)
add_subdirectory(libecc)
add_subdirectory(libnetwork)
add_subdirectory(libficfg)
#add_subdirectory(simulation)
add_subdirectory(voter)
add_subdirectory(controller)
add_subdirectory(tests)
add_subdirectory(test_programms)
add_subdirectory(libalgo)

