cmake_minimum_required(VERSION 2.8)
project(simulation)

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11 -D_POSIX_C_SOURCE=200112L -g")
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG")
#set(LIBS "-lm")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LIBS}")

set(CXX_WARNINGS "-Wall -Wextra -Wpedantic -Wformat=2 -Wmissing-include-dirs -Wswitch-default -Wuninitialized -Wfloat-equal -Wundef -Wshadow -Wcast-qual -Wcast-align -Wconversion -Wjump-misses-init -Wlogical-op -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Winline -Wdisabled-optimization -Wstrict")

set(SOURCE_FILES_SIMULATION main.c)
add_executable(simulation ${SOURCE_FILES_SIMULATION})
target_link_libraries(simulation m)
