#include "spawner.h"

int get_number_of_commands(char **command, int size_command);

char** remove_ampercand(char **command, int size_command);

void print_commands(char **coms, int size);

char** copy_command(char** newcommand, char** oldcommand, int size);

struct command_pipe_t
{
    char ***command_list;
    int *command_lengths;
    int num_commands;
    bool async;
}; 

typedef struct command_pipe_t* Piper;

Piper create_piper(char **command, int size_command);

int run_commands(Piper p);

int _run_sync_commands(Piper p);

int _run_async_commands(Piper p);

void free_piper(Piper p);
