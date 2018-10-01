#ifndef COMPILE_H
#define COMPILE_H

/* compile.h
 * Author: Ian Lumsden
 *
 * This file contains the declarations of the struct and functions used in
 * fakemake.c to perform compilation.
 */

// Added to maintain the definition of POSIX functions in C99
#define _POSIX_C_SOURCE 200809L

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

// Basic macros to define maximum number of command-line arguments,
// max size of the executable name, and max size of a single command string.
// These are simply used to simplify coding.
#define MAX_ARGS 64
#define MAX_EXEC_NAME 512
#define MAX_COMPILE_COMMAND_SIZE 2048

// This struct stores arrays of strings representing the different 
// gcc compilation commands described by the input file.
// The ints are simply used to store the number of each component.
typedef struct compile_t
{
    char  *exec_name;
    char **srcs;
    int    num_srcs;
    char **headers;
    int    num_headers;
    char **flag_list;
    int    num_flags;
    char **libraries;
    int    num_libs;
    int    num_commands;
} Compile;

// Allocates the memory for a new Compile object and returns a pointer to it.
extern Compile* new_compilation();

// Adds the contents of a C line from the input file to the Compile struct
extern void add_srcs(Compile *compilation, char **fields, int NF);

// Adds the contents of an E line from the input file to the Compile struct
extern void add_exec(Compile *compilation, char **fields, int NF, int linenum);

// Adds the contents of a H line from the input file to the Compile struct
extern void add_headers(Compile *compilation, char **fields, int NF);

// Adds the contents of a F line from the input file to the Compile struct
extern void add_flags(Compile *compilation, char **fields, int NF);

// Adds the contents of a L line from the input file to the Compile struct
extern void add_libraries(Compile *compilation, char **fields, int NF);

// Generates an array of strings containing the gcc compilation commands that need
// to be run to create the executable
extern char** get_commands(Compile *compilation);

// Frees the memory for the provided Compile object
extern void free_compilation(Compile *compilation);

#endif
