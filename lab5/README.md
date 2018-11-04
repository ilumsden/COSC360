# Lab 5 Notes:

The `makefile` for this lab is a wrapper for CMake. CMake will compile the following:
* The contents of the main `lab5` directory into `libjmalloc.so`
* The contents of `lab1` into `famtree`
* The contents of `malloc_test` into `test_malloc`

## libjmalloc.so

This library contains all the code written explicitly for this lab. The files compiled into this library are:
* `malloc.c/h`
* `freelist.c/h`
* `heap_print.c/h`

The `freelist.c` file implements all the underlying logic for the `malloc` functions. `malloc.c` uses the functions in `freelist.c` to implement the `malloc` functions. `heap_print.c` is not a required part of the lab, but it was implemented to simplify printing the contents of the heap and stack for testing purposes.

## famtree

This executable is produced by the code in the `lab1` directory. This code is a copy of my solution to `lab1`, but it is slightly reworked to use my implementation of the `malloc` functions.

## test_malloc

This executable is produced by the code in the `malloc_test` directory. This code uses my `malloc` functions to allocate several blocks of memory. It then uses the code included from `heap_print.c/h` to print the heap and stack to `stdout`.
