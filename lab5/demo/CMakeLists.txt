cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

project(CS360Lab5Demo)
enable_language(C)

option(PERSONAL "Builds lab 5's demo for a non-Hydra computer." OFF)

#if(PERSONAL)
    #message(STATUS "Building fdr")
    #file(GLOB fdr_src "${PROJECT_SOURCE_DIR}/../libfdr/src/*.c")
    #message(STATUS "fdr_src is ${fdr_src}")
    #file(GLOB fdr_headers "${PROJECT_SOURCE_DIR}/../libfdr/include/*.h")
    #if(NOT TARGET fdr)
        #add_library(fdr STATIC ${fdr_src} ${fdr_headers})
    #endif()
#endif()

file(GLOB demo_src "${PROJECT_SOURCE_DIR}/*.c")
file(GLOB demo_headers "${PROJECT_SOURCE_DIR}/*.h")

if(PERSONAL)
    include_directories(${PROJECT_SOURCE_DIR}/../libfdr/include)
else()
    include_directories(/home/plank/cs360/include)
endif()

add_executable(demo ${demo_src} ${demo_headers})

target_link_libraries(demo PRIVATE jmalloc)

#if(PERSONAL)
    #target_link_libraries(demo PRIVATE fdr)
#else()
    #target_link_libraries(demo PRIVATE /home/plank/cs360/objs/libfdr.a)
#endif()
