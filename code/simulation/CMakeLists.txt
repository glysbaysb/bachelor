#cmake_minimum_required(VERSION 3.3)
project(simulation)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -D_POSIX_C_SOURCE=200112L")
set(CMAKE_C_FLAGS_DEBUG "-DDEBUG")

set(WARNINGS "")
if(${WARNINGS})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
        -Wall -Wextra -Wpedantic -Wformat=2 -Wmissing-include-dirs \
        -Wswitch-default -Wuninitialized -Wfloat-equal -Wundef -Wshadow \
        -Wcast-qual -Wcast-align -Wconversion -Wjump-misses-init \
        -Wlogical-op -Wmissing-prototypes -Wmissing-declarations \
        -Wredundant-decls -Winline -Wdisabled-optimization \
		-Wstrict")
endif()

set(SOURCE_FILES_SIMULATION main.c)
add_executable(simulation ${SOURCE_FILES_SIMULATION})
