cmake_minimum_required(VERSION 3.2)
project(tlc_sec)

set(CMAKE_C_STANDARD 99)
SET(CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
SET(CMAKE_C_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")

set(SOURCE_FILES main.c)
add_executable(tlc_sec ${SOURCE_FILES})

add_subdirectory(crypto)
add_subdirectory(tests)