cmake_minimum_required(VERSION 2.8)
project(libficfg)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_WARNINGS_PRIV}")

# Declare the library
set(SOURCE_FILES_LIBFICFG
	ficfg.cxx ficfg.h)
add_library(ficfg STATIC
	${SOURCE_FILES_LIBFICFG}
)
target_link_libraries(ficfg)
# Specify the include directories exported by this library
target_include_directories(ficfg PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

