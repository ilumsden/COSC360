#include "spawner.h"

// Determines how many commands are piped together in the user input.
int get_number_of_commands(char **command, int size_command);

// Removes the ampercand at the end of the user input.
char** remove_ampercand(char **command, int size_command);

// Prints the contents of a command
// This function was used for debugging purposes only.
void print_commands(char **coms, int size);

// Defines the struct for containing the user input separated by piping.
// command_list is an array of arrays of strings. Each subarray represents a command
//     in a sequence of piped commands.
// command_lengths is an array of ints that stores the number of strings in the corresponding
//     subarray in command_list.
// num_commands is the number of pipeed commands (the number of subarrays in command_list)
// async states whether the shell should wait for the current command to finish.
//     It is able to be a bool because this code is compiled with C99. The stdbool.h header
//     is included in spawner.h
struct command_pipe_t
{
    char ***command_list;
    int *command_lengths;
    int num_commands;
    bool async;
}; 

typedef struct command_pipe_t* Piper;

// Constructs a Piper from the vectorized input string and the number of words in the input.
Piper create_piper(char **command, int size_command);

// Runs the commands stored in the passed Piper.
int run_commands(Piper p);

// A helper function for running a command or sequence of commands that the shell should wait on
int _run_sync_commands(Piper p);

// A helper function for running a command or sequence of commands that the shell should not wait on
int _run_async_commands(Piper p);

// Frees the memory in the passed Piper.
void free_piper(Piper p);
