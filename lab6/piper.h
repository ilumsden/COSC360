#include "spawner.h"

int get_number_of_commands(char **command, int size_command);

void print_commands(char **coms, int size);

typedef struct command_pipe_t
{
    char ***command_list;
    int *command_lengths;
    int num_commands;
} *Piper;

Piper create_piper(char **command, int size_command);

int run_commands(Piper p);

void free_piper(Piper p);
