cmake_minimum_required(VERSION 2.8)
project(ficfg_test)

set(SOURCE_FILES_FICFG test_ficfg.cxx)
add_executable(test_ficfg ${SOURCE_FILES_FICFG})
target_link_libraries(test_ficfg
	gtest
	gmock
	ficfg
)
