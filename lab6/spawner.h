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

void cd(char *path);

void spawn_synchronous_process(char **newargv, const char *inpipe, 
        const char *outpipe, const char *appendpipe, int infd, int outfd, bool firstproc,
        bool finalproc);

void spawn_asynchronous_process(char **newargv, int num_coms, const char *inpipe,
        const char *outpipe, const char *appendpipe, int infd, int outfd, bool firstproc,
        bool finalproc);

int find_input_pipe(char **newargv, int size_newargv);

int find_output_pipe(char **newargv, int size_newargv);

int find_append_pipe(char **newargv, int size_newargv);

char** remove_single_pipe(char **newargv, int *size_newargv, int pipe_ind);

#endif
