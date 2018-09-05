cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

project(CS360Labs LANGUAGES C)

#option(LAB1 "Build Lab 1." OFF)

file(GLOB fdr_src "${CMAKE_SOURCE_DIR}/libfdr/*.c")
file(GLOB fdr_headers "${CMAKE_SOURCE_DIR}/libfdr/*.h")

add_library(fdr STATIC ${fdr_src} ${fdr_headers})

file(GLOB lab1_src "${CMAKE_SOURCE_DIR}/lab1/*.c")
file(GLOB lab1_headers "${CMAKE_SOURCE_DIR}/lab1/*.h")

include_directories(${CMAKE_SOURCE_DIR}/libfdr)

add_executable(famtree ${lab1_src} ${lab1_headers})
target_link_libraries(famtree PRIVATE fdr)
