cmake_minimum_required(VERSION 3.5)

project(HomeWork_4)

file(GLOB source_files "./*.c")
add_executable(cond_var ${source_files})

file(GLOB queue_files "./queue/*.c")
add_library(queue ${queue_files})
target_include_directories(queue PRIVATE "./include")

target_include_directories(cond_var PRIVATE "./include")
target_link_libraries(cond_var PRIVATE pthread)
target_link_libraries(cond_var PRIVATE queue)