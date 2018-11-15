#ifndef SPAWNER_H
#define SPAWNER_H

#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void cd(char *path);

void spawn_synchronous_process(char **newargv, const char *inpipe, 
        const char *outpipe, const char *appendpipe);

void spawn_asynchronous_process(char **newargv, int num_coms, const char *inpipe,
        const char *outpipe, const char *appendpipe);

int find_input_pipe(char **newargv);

int find_output_pipe(char **newargv);

int find_append_pipe(char **newargv);

//void spawn_input_piped_process(char **newargv, char *inputfile);

//void spawn_output_piped_process(char **newargv, char *outputfile);

#endif
