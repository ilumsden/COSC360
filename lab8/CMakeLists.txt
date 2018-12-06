cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

project(CS360Lab8)
enable_language(C)

if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
        set (CMAKE_C_FLAGS "-std=c99 ${CMAKE_C_FLAGS}")
    endif()
else()
    set (CMAKE_C_STANDARD 99)
endif()

set(CMAKE_C_FLAGS "-Wall -g -O ${CMAKE_C_FLAGS}")

file(GLOB lab8_src "${PROJECT_SOURCE_DIR}/*.c")
file(GLOB lab8_headers "${PROJECT_SOURCE_DIR}/*.h")

set(SOCK_LIB "/home/plank/cs360/objs/socketfun.o")

include_directories(/home/plank/cs360/include)

add_executable(jtalk ${lab8_src} ${lab8_headers} ${SOCK_LIB})

find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
    target_compile_options(jtalk PUBLIC "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
    target_link_libraries(jtalk "${CMAKE_THREAD_LIBS_INIT}")
endif()

target_link_libraries(fakemake PRIVATE /home/plank/cs360/objs/libfdr.a)
