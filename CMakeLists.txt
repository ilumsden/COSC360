cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

project(CS360)
enable_language(C)

option(PERSONAL "Builds Labs for a non-Hydra computer." OFF)

option(ALL_LABS "Builds All Labs." ON)
option(LAB1 "Builds Lab 1." OFF)
option(LAB2 "Builds Lab 2." OFF)

if(LAB1)
    message(STATUS "In Lab1")
    add_subdirectory(lab1)
elseif(LAB2)
    message(STATUS "In Lab2")
    add_subdirectory(lab2)
else()
    message(STATUS "In else")
    add_subdirectory(lab1)
    add_subdirectory(lab2)
endif()
