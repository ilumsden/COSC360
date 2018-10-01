# My system for compilation requires CMake.
# In fact, my makefile is just a basic wrapper for the CMake compilation commands.
# As a result, this file is included in the submissions directory.
#
# To compile, just use the makefile as you normally would.

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

project(CS360Lab3)
enable_language(C)

option(PERSONAL "Builds lab 3 for a non-Hydra computer." OFF)

if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
        set (CMAKE_C_FLAGS "-std=c99 ${CMAKE_C_FLAGS}")
    endif()
else()
    set (CMAKE_C_STANDARD 99)
endif()

set (CMAKE_C_FLAGS "-Wall -g -O ${CMAKE_C_FLAGS}")

if(PERSONAL)
    message(STATUS "Building fdr")
    file(GLOB fdr_src "${PROJECT_SOURCE_DIR}/../libfdr/src/*.c")
    file(GLOB fdr_headers "${PROJECT_SOURCE_DIR}/../libfdr/include/*.h")
    if(NOT TARGET fdr)
        add_library(fdr STATIC ${fdr_src} ${fdr_headers})
    endif()
endif()

file(GLOB lab3_src "${PROJECT_SOURCE_DIR}/*.c")
file(GLOB lab3_headers "${PROJECT_SOURCE_DIR}/*.h")

if(PERSONAL)
    include_directories(${PROJECT_SOURCE_DIR}/../libfdr/include)
else()
    include_directories(/home/plank/cs360/include)
endif()

add_executable(fakemake ${lab3_src} ${lab3_headers})

if(PERSONAL)
    target_link_libraries(fakemake PRIVATE fdr)
else()
    target_link_libraries(fakemake PRIVATE /home/plank/cs360/objs/libfdr.a)
endif()
