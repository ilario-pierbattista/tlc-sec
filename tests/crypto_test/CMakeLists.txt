include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(../../crypto)

file(GLOB_RECURSE TestSrc *.cpp)

add_executable(run_tests ${TestSrc})
add_dependencies(run_tests lib des)

target_link_libraries(run_tests gtest gtest_main lib des)