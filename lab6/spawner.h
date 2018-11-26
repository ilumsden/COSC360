#ifndef SPAWNER_H
#define SPAWNER_H

#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// My personal implementation of waitpid
pid_t pidwait(pid_t pid, int *stats);

// Changes directory to the one specified by path
void cd(char *path);

// Creates a new process with input and output redirected based on inpipe, outpipe, appendpipe,
// infd, outfd, firstproc, and finalproc
void spawn_synchronous_process(char **newargv, const char *inpipe, 
        const char *outpipe, const char *appendpipe, int infd, int outfd, bool firstproc,
        bool finalproc);

// Finds the index of < in newargv
int find_input_pipe(char **newargv, int size_newargv);

// Finds the index of > in newargv
int find_output_pipe(char **newargv, int size_newargv);

// Finds the index of >> in newargv
int find_append_pipe(char **newargv, int size_newargv);

// Removes <, >, or >> and the connected file name from newargv
char** remove_single_pipe(char **newargv, int *size_newargv, int pipe_ind);

#endif
