cmake_minimum_required(VERSION 3.6)
project(tlc_sec)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c)
add_executable(tlc_sec ${SOURCE_FILES})

add_subdirectory(crypto)
add_subdirectory(tests)