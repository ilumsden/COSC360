#include "piper.h"

int get_number_of_commands(char **command, int size_command)
{
    // Increments the count for every piping symbol in the vectorized input.
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
    // Adds one to the number of piping symbols because there will always be one more
    // command than piping symbol.
    return num_pipes + 1;
}

char** remove_ampercand(char **command, int size_command)
{
    // Copies everything from command into newcommand except for the ampercand.
    char **newcommand = (char**) malloc((size_command-1)*sizeof(char*));
    for (int i = 0; i < size_command-1; i++)
    {
        if (command[i] == NULL)
        {
            continue;
        }
        if (i != size_command - 2)
        {
            newcommand[i] = command[i];
        }
    }
    newcommand[size_command-2] = NULL;
    return newcommand;
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
    Piper p = (Piper) malloc(sizeof(struct command_pipe_t));
    char **newcommand;
    int newsize;
    bool async;
    // Sets newcommand, newsize, and async based on whether there is an ampercand
    // at the end of command.
    if (size_command >= 2 && strcmp(command[size_command-2], "&") == 0)
    {
        async = true;
        newcommand = remove_ampercand(command, size_command);
        newsize = size_command - 1;
    }
    else
    {
        async = false;
        newsize = size_command;
        newcommand = command;
    }
    // Gets the number of piped commands and uses the number to allocate space for
    // command_list and cl (essentially command_lengths).
    int nc = get_number_of_commands(newcommand, newsize);
    p->command_list = (char***) malloc(nc*sizeof(char**));
    int *cl = (int*) malloc(nc*sizeof(int));
    // Simply assigns some variables if the input includes no pipes.
    if (nc == 1)
    {
        p->command_list[0] = newcommand;
        p->command_lengths = (int*) malloc(nc*sizeof(int));
        p->command_lengths[0] = newsize;
        p->num_commands = nc;
        p->async = async;
        free(cl);
        return p;
    }
    // com_ind is used to keep track of the current index in command_list.
    int com_ind = 0;
    // prev_ind is used to keep track of the index of the beginning of the current command.
    int prev_ind = 0;
    for (int i = 0; i < newsize; i++)
    {
        if (newcommand[i] == NULL)
        {
            continue;
        }
	// When the loop reaches a piping symbol, the strings making up the command are copied into command_list.
        if (strcmp(newcommand[i], "|") == 0)
        {
            p->command_list[com_ind] = (char**) malloc((i-prev_ind+1)*sizeof(char*));
            cl[com_ind] = i - prev_ind + 1;
            for (int j = 0; j < i-prev_ind; j++)
            {
                p->command_list[com_ind][j] = newcommand[prev_ind + j];
            }
            p->command_list[com_ind][cl[com_ind]-1] = NULL;
            com_ind++;
            prev_ind = i+1;
        }
    }
    // Copies the last command into command_list
    p->command_list[com_ind] = (char**) malloc((newsize-prev_ind)*sizeof(char*));
    cl[com_ind] = newsize - prev_ind;
    for (int j = 0; j < newsize-prev_ind; j++)
    {
        p->command_list[com_ind][j] = newcommand[prev_ind + j];
    }
    // Copies the remainder of the data into the Piper.
    p->command_lengths = cl;
    p->num_commands = nc;
    p->async = async;
    if (async)
    {
        free(newcommand);
    }
    return p;
}

int run_commands(Piper p)
{
    // Passes off running the commands to helper functions based on whether
    // there was an ampercand at the end.
    int retval;
    if (p->async)
    {
        retval = _run_async_commands(p);
    }
    else
    {
        retval = _run_sync_commands(p);
    }
    return retval;
}

int _run_sync_commands(Piper p)
{
    // Opens a new pipe
    int currpipe[2];
    if ( pipe(currpipe) == -1 )
    {
        fprintf(stderr, "Error: could not create pipe. Aborting command.\n");
        return -1;
    }
    // in is used to store the read end of the previous pipe.
    int in = 0;
    for (int i = 0; i < p->num_commands; i++)
    {
        char **command = p->command_list[i];
        bool shouldfree = false;
	// If the current command is cd, changes directory using the cd function from
	// spawner.h
        if (strcmp(command[0], "cd") == 0)
        {
            cd(command[1]);
            return 0;
        }
	// Stores the file names for file redirect for input, output, and append.
	// The redirects are removed once the file names are obtained.
        bool first = true;
        char *inpipe = NULL;
        char *outpipe = NULL;
        char *appipe = NULL;
        int pind = find_input_pipe(command, p->command_lengths[i]);
        if (pind != -1 && i == 0)
        {
            inpipe = command[pind+1];
            command = remove_single_pipe(command, &(p->command_lengths[i]), pind);
            first = false;
            shouldfree = true;
        }
        pind = find_output_pipe(command, p->command_lengths[i]);
        if (pind != -1 && i == p->num_commands-1)
        {
            char **tmp;
            if (!first)
            {
                tmp = command;
            }
            outpipe = command[pind+1];
            command = remove_single_pipe(command, &(p->command_lengths[i]), pind);
            if (!first)
            {
                free(tmp);
            }
            first = false;
            shouldfree = true;
        }
        pind = find_append_pipe(command, p->command_lengths[i]);
        if (pind != -1 && i == p->num_commands-1)
        {
            char **tmp;
            if (!first)
            {
                tmp = command;
            }
            appipe = command[pind+1];
            command = remove_single_pipe(command, &(p->command_lengths[i]), pind);
            if (!first)
            {
                free(tmp);
            }
            first = false;
            shouldfree = true;
        }
	// If the command is exit, the return value is propagated to the main function, and the
	// shell controll loop is broken.
        if (strcmp(command[0], "exit") == 0)
        {
            return 1;
        }
	// The if statement ensures there is not an ampercand at the end of the command.
        else if (strcmp(command[p->command_lengths[i]-2], "&") != 0)
        {
	    // Stores a pipe read end in "in", runs the process, and closes the old pipe.
            if (p->num_commands == 1 || i == p->num_commands-1)
            {
                if (p->num_commands == 1)
                {
                    in = currpipe[0];
                }
                spawn_synchronous_process(command, inpipe, outpipe, appipe, in, currpipe[1], (i==0), true);
                if (in != 0)
                {
                    close(in);
                }
                close(currpipe[0]);
                close(currpipe[1]);
            }
            else if (i == 0)
            {
                spawn_synchronous_process(command, inpipe, outpipe, appipe, currpipe[0], currpipe[1], true, false);
                in = currpipe[0];
                close(currpipe[1]);
                if ( pipe(currpipe) == -1 )
                {
                    fprintf(stderr, "Error: could not create pipe. Aborting command.\n");
                    return -1;
                }
            }
            else
            {
                spawn_synchronous_process(command, inpipe, outpipe, appipe, in, currpipe[1], false, false);
                close(in);
                in = currpipe[0];
                close(currpipe[1]);
                if ( pipe(currpipe) == -1 )
                {
                    fprintf(stderr, "Error: could not create pipe. Aborting command.\n");
                    return -1;
                }
            }
        }
	// Raises an error because an async command should not get here.
        else if (strcmp(command[p->command_lengths[i]-2], "&") == 0)
        {
            fprintf(stderr, "jsh: syntax error near unexpected token \'|\'\n");
            return -1;
        }
        else
        {
            fprintf(stderr, "Invalid input.\n");
            return 0;
        }
	// Free's the command if it needs to be.
        if (shouldfree)
        {
            free(command);
        }
    }
    return 0;
}

int _run_async_commands(Piper p)
{
    // Produces a fork. In the new process, _run_sync_commands is run.
    // The current process does not wait.
    int pid;
    pid = fork();
    if (pid == 0)
    {
        _run_sync_commands(p);
        exit(0);
    }
    else if (pid == -1)
    {
        fprintf(stderr, "Error: could not run the provided command\n");
        return -1;
    }
    else
    {
        return 0;
    }
}

void free_piper(Piper p)
{
    for (int i = 0; i < p->num_commands; i++)
    {
        free(p->command_list[i]);
    }
    free(p->command_list);
    free(p->command_lengths);
    free(p);
}
