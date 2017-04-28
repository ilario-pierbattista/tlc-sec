include_directories(lib)
include_directories(des)

file(GLOB_RECURSE libsrc lib/*.*)
add_library(lib ${libsrc})

file(GLOB_RECURSE DESsrc des/*.*)
add_library(des ${DESsrc})
target_link_libraries(des lib)

add_executable(des_example examples/des_example.c)
target_link_libraries(des_example des)