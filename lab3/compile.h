#ifndef COMPILE_H
#define COMPILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fields.h"

#define MAX_ARGS 64
#define MAX_EXEC_NAME 512

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
} Compile;

extern Compile* new_compilation();

extern void add_srcs(Compile *compilation, char **fields, int NF);

extern void add_exec(Compile *compilation, char **fields, int NF);

extern void add_headers(Compile *compilation, char **fields, int NF);

extern void add_flags(Compile *compilation, char **fields, int NF);

extern void add_libraries(Compile *compilation, char **fields, int NF);

extern char** get_commands(Compile *compilation);

extern void free_compilation(Compile *compilation);

#endif
