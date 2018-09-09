cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

project(CS360)
enable_language(C)

option(PERSONAL "Builds Labs for a non-Hydra computer." OFF)

add_subdirectory(lab1)
