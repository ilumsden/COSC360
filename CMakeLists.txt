cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

project(CS360Labs)
enable_language(C)

option(PERSONAL "Builds the labs for a non-Hydra computer." OFF)
#option(LAB1 "Build Lab 1." OFF)

if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
        set (CMAKE_C_FLAGS "-std=c99 ${CMAKE_C_FLAGS}")
    endif()
else()
    set (CMAKE_C_STANDARD 99)
endif()

set (CMAKE_C_FLAGS "-Wall -g -O ${CMAKE_C_FLAGS}")

if(PERSONAL)
    file(GLOB fdr_src "${CMAKE_SOURCE_DIR}/libfdr/*.c")
    file(GLOB fdr_headers "${CMAKE_SOURCE_DIR}/libfdr/*.h")
    add_library(fdr STATIC ${fdr_src} ${fdr_headers})
endif()

file(GLOB lab1_src "${CMAKE_SOURCE_DIR}/lab1/*.c")
file(GLOB lab1_headers "${CMAKE_SOURCE_DIR}/lab1/*.h")

if(PERSONAL)
    include_directories(${CMAKE_SOURCE_DIR}/libfdr)
else()
    include_directories(/home/plank/cs360/include)
endif()

add_executable(famtree ${lab1_src} ${lab1_headers})

if(PERSONAL)
    target_link_libraries(famtree PRIVATE fdr)
else()
    target_link_libraries(famtree PRIVATE /home/plank/cs360/objs/libfdr.a)
endif()
