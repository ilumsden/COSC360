cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

project(CS360Lab5Demo)
enable_language(C)

option(PERSONAL "Builds lab 5's test for malloc for a non-Hydra computer." OFF)

if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
        set (CMAKE_C_FLAGS "-std=c99 ${CMAKE_C_FLAGS}")
    endif()
else()
    set (CMAKE_C_STANDARD 99)
endif()

set (CMAKE_C_FLAGS "-Wall -g -O -fno-builtin ${CMAKE_C_FLAGS}")

file(GLOB demo_src "${PROJECT_SOURCE_DIR}/*.c")
file(GLOB demo_headers "${PROJECT_SOURCE_DIR}/*.h")

add_executable(test_malloc ${demo_src} ${demo_headers})

target_link_libraries(test_malloc PRIVATE jmalloc)
