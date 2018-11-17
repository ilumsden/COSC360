#include "piper.h"

int get_number_of_commands(char **command, int size_command)
{
    int num_pipes = 0;
    for (int i = 0; i < size_command; i++)
    {
        if (command[i] == NULL)
        {
            continue;
        }
        if (strcmp(command[i], "|") == 0)
        {
            num_pipes++;
        }
    }
    return num_pipes + 1;
}

void print_commands(char **coms, int size)
{
    char **iter = coms;
    printf("Num commands is %d\n", size);
    for (int i = 0; i < size; i++)
    {
        if (iter[i] != NULL)
        {
            printf("%s\n", iter[i]);
        }
        else
        {
            printf("(null)\n");
        }
    }
}

Piper create_piper(char **command, int size_command)
{
    Piper p = (Piper) malloc(sizeof(Piper));
    p->num_commands = get_number_of_commands(command, size_command);
    p->command_list = (char***) malloc(p->num_commands*sizeof(char**));
    p->command_lengths = (int*) malloc(p->num_commands*sizeof(int));
    if (p->num_commands == 1)
    {
        p->command_list[0] = command;
        p->command_lengths[0] = size_command;
        return p;
    }
    int com_ind = 0;
    int prev_ind = 0;
    for (int i = 0; i < size_command; i++)
    {
        if (command[i] == NULL)
        {
            continue;
        }
        if (strcmp(command[i], "|") == 0)
        {
            p->command_list[com_ind] = (char**) malloc((i-prev_ind+1)*sizeof(char*));
            p->command_lengths[com_ind] = i - prev_ind + 1;
            for (int j = 0; j < i-prev_ind; j++)
            {
                p->command_list[com_ind][j] = command[prev_ind + j];
            }
            p->command_list[com_ind][i-prev_ind] = NULL;
            com_ind++;
            prev_ind = i+1;
        }
    }
    p->command_list[com_ind] = (char**) malloc((size_command-prev_ind)*sizeof(char*));
    for (int j = 0; j < size_command-prev_ind; j++)
    {
        p->command_list[com_ind][j] = command[prev_ind + j];
    }
    return p;
}

int run_commands(Piper p)
{
    int currpipe[2];
    int nextpipe[2];
    currpipe[0] = -1;
    currpipe[1] = -1;
    if (p->num_commands == 1)
    {
        nextpipe[0] = -1;
        nextpipe[1] = -1;
    }
    else
    {
        if ( pipe(nextpipe) == -1 )
        {
            fprintf(stderr, "Error: could not create pipe. Aborting command.\n");
            return -1;
        }
    }
    for (int i = 0; i < p->num_commands; i++)
    {
        char **command = p->command_list[i];
        if (strcmp(command[0], "cd") == 0)
        {
            cd(command[1]);
            return 0;
        }
        char *inpipe = NULL;
        char *outpipe = NULL;
        char *appipe = NULL;
        int pind = find_input_pipe(command, p->command_lengths[i]);
        if (pind != -1 && i == 0)
        {
            inpipe = command[pind+1];
            command = remove_single_pipe(command, &(p->command_lengths[i]), pind);
        }
        pind = find_output_pipe(command, p->command_lengths[i]);
        if (pind != -1 && i == p->num_commands-1)
        {
            outpipe = command[pind+1];
            command = remove_single_pipe(command, &(p->command_lengths[i]), pind);
        }
        pind = find_append_pipe(command, p->command_lengths[i]);
        if (pind != -1 && i == p->num_commands-1)
        {
            appipe = command[pind+1];
            command = remove_single_pipe(command, &(p->command_lengths[i]), pind);
        }
        print_commands(command, p->command_lengths[i]);
        if (strcmp(command[0], "exit") == 0)
        {
            return 1;
        }
        else if (strcmp(command[p->command_lengths[i]-2], "&") != 0)
        {
            spawn_synchronous_process(command, inpipe, outpipe, appipe, currpipe[0], nextpipe[1]);
        }
        else if (strcmp(command[p->command_lengths[i]-2], "&") == 0)
        {
            spawn_asynchronous_process(command, p->command_lengths[i], inpipe, 
                    outpipe, appipe, currpipe[0], nextpipe[1]);
        }
        else
        {
            fprintf(stderr, "Invalid input.\n");
            return 0;
        }
        currpipe[0] = nextpipe[0];
        currpipe[1] = nextpipe[1];
        if (p->num_commands != 1 && i == p->num_commands - 2)
        {
            nextpipe[0] = -1;
            nextpipe[1] = -1;
        }
        else
        {
            if ( pipe(nextpipe) == -1 )
            {
                fprintf(stderr, "Error: could not create pipe. Aborting command.\n");
                return -1;
            }
        }
    }
    return 0;
}

void free_piper(Piper p)
{
    free(p->command_lengths);
    for (int i = 0; i < p->num_commands; i++)
    {
        free(p->command_list[i]);
    }
    free(p->command_list);
    free(p);
}
