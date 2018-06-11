cmake_minimum_required(VERSION 3.0)

project(liger-console)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(TEST_EXE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests-bin)
find_package(Threads)

add_subdirectory (src)
add_subdirectory (tests)
