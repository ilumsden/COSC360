cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

project(CS360Lab6)
enable_language(C)

option(PERSONAL "Builds lab 6 for a non-Hydra computer." OFF)

if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
        set (CMAKE_C_FLAGS "-std=gnu99 ${CMAKE_C_FLAGS}")
    endif()
else()
    set (CMAKE_C_STANDARD 99)
endif()

set (CMAKE_C_FLAGS "-Wall -g -O ${CMAKE_C_FLAGS}")

file(GLOB lab6_src "${PROJECT_SOURCE_DIR}/*.c")
file(GLOB lab6_headers "${PROJECT_SOURCE_DIR}/*.h")

add_executable(jsh ${lab6_src} ${lab6_headers})
