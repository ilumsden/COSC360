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
list(REMOVE_ITEM lab8_src "${PROJECT_SOURCE_DIR}/jtalk.c")
message(STATUS "Src files are ${lab8_src}")
file(GLOB lab8_headers "${PROJECT_SOURCE_DIR}/*.h")

#set(SOCK_LIB "/home/plank/cs360/objs/socketfun.o")
add_library(libfdr SHARED "/home/plank/cs360/include/dllist.h" "/home/plank/cs360/include/fields.h" "/home/plank/cs360/include/jrb.h" "/home/plank/cs360/include/jval.h" "/home/plank/cs360/src/dllist.c" "/home/plank/cs360/src/fields.c" "/home/plank/cs360/src/jrb.c" "/home/plank/cs360/src/jval.c")
#set(LIB_FDR "/home/plank/cs360/objs/libfdr.a")

include_directories(/home/plank/cs360/include)

#add_library(socketfun SHARED "/home/plank/cs360/include/socketfun.h" "/home/plank/cs360/src/socketfun.c")
add_library(socketfun SHARED "/home/plank/cs360/include/socketfun.h" "socketfun.c")

#add_executable(jtalk_server ${lab8_src} ${lab8_headers} ${SOCK_LIB})
add_executable(jtalk_server ${lab8_src} ${lab8_headers})
#add_executable(jtalk "jtalk.c" ${SOCK_LIB})
add_executable(jtalk "jtalk.c")

target_link_libraries(jtalk PRIVATE socketfun)
target_link_libraries(jtalk_server PRIVATE socketfun)

find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
    target_compile_options(jtalk PRIVATE "-pthread")
    target_compile_options(jtalk_server PRIVATE "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
    target_link_libraries(jtalk PRIVATE "${CMAKE_THREAD_LIBS_INIT}")
    target_link_libraries(jtalk_server PRIVATE "${CMAKE_THREAD_LIBS_INIT}")
endif()

#target_link_libraries(jtalk PRIVATE ${LIB_FDR})
#target_link_libraries(jtalk_server PRIVATE ${LIB_FDR})
target_link_libraries(jtalk PRIVATE libfdr)
target_link_libraries(jtalk_server PRIVATE libfdr)
