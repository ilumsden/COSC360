cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

project(CS360Labs)
enable_language(C)

option(PERSONAL "Builds the labs for a non-Hydra computer." OFF)
#option(LAB1 "Build Lab 1." OFF)

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
set_property(TARGET famtree PROPERTY C_STANDARD 99)

if(PERSONAL)
    target_link_libraries(famtree PRIVATE fdr)
else()
    target_link_libraries(famtree PRIVATE /home/plank/cs360/objs/libfdr.a)
endif()
