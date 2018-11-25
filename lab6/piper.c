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

char** remove_ampercand(char **command, int size_command)
{
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

char** copy_command(char** newcommand, char** oldcommand, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (oldcommand[i] != NULL)
        {
            newcommand[i] = oldcommand[i];
        }
        else
        {
            newcommand[i] = NULL;
        }
    }
    return newcommand;
}

Piper create_piper(char **command, int size_command)
{
    Piper p = (Piper) malloc(sizeof(struct command_pipe_t));
    char **newcommand;
    int newsize;
    bool async;
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
        newcommand = (char**) malloc(newsize*sizeof(char*));
        newcommand = copy_command(newcommand, command, newsize);
        //newcommand = command;
    }
    int nc = get_number_of_commands(newcommand, newsize);
    p->command_list = (char***) malloc(nc*sizeof(char**));
    int *cl = (int*) malloc(nc*sizeof(int));
    if (nc == 1)
    {
        p->command_list[0] = (char**) malloc(newsize*sizeof(char*));
        p->command_list[0] = copy_command(p->command_list[0], newcommand, newsize);
        p->command_lengths = (int*) malloc(nc*sizeof(int));
        p->command_lengths[0] = newsize;
        p->num_commands = nc;
        p->async = async;
        free(cl);
        free(newcommand);
        return p;
    }
    int com_ind = 0;
    int prev_ind = 0;
    for (int i = 0; i < newsize; i++)
    {
        if (newcommand[i] == NULL)
        {
            continue;
        }
        if (strcmp(newcommand[i], "|") == 0)
        {
            p->command_list[com_ind] = (char**) malloc((i-prev_ind+1)*sizeof(char*));
            cl[com_ind] = i - prev_ind + 1;
            p->command_list[com_ind] = copy_command(p->command_list[com_ind], 
                    &(newcommand[prev_ind]), cl[com_ind]-1);
            print_commands(p->command_list[com_ind], cl[com_ind]);
            /*for (int j = 0; j < i-prev_ind; j++)
            {
                p->command_list[com_ind][j] = newcommand[prev_ind + j];
            }*/
            p->command_list[com_ind][i-prev_ind] = NULL;
            com_ind++;
            prev_ind = i+1;
        }
    }
    p->command_list[com_ind] = (char**) malloc((newsize-prev_ind)*sizeof(char*));
    cl[com_ind] = newsize - prev_ind;
    p->command_list[com_ind] = copy_command(p->command_list[com_ind], 
            &(newcommand[prev_ind]), cl[com_ind]-1);
    /*for (int j = 0; j < newsize-prev_ind; j++)
    {
        p->command_list[com_ind][j] = newcommand[prev_ind + j];
    }*/
    p->command_lengths = cl;
    p->num_commands = nc;
    p->async = async;
    free(newcommand);
    return p;
}

int run_commands(Piper p)
{
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
    int currpipe[2];
    if ( pipe(currpipe) == -1 )
    {
        fprintf(stderr, "Error: could not create pipe. Aborting command.\n");
        return -1;
    }
    int in = 0;
    for (int i = 0; i < p->num_commands; i++)
    {
        char **command = p->command_list[i];
        bool shouldfree = false;
        if (strcmp(command[0], "cd") == 0)
        {
            cd(command[1]);
            return 0;
        }
        bool first = true;
        char *inpipe = NULL;
        char *outpipe = NULL;
        char *appipe = NULL;
        int pind = find_input_pipe(command, p->command_lengths[i]);
        if (pind != -1 && i == 0)
        {
            inpipe = command[pind+1];
            command = remove_single_pipe(command, &(p->command_lengths[i]), pind);
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
            shouldfree = true;
        }
        if (strcmp(command[0], "exit") == 0)
        {
            return 1;
        }
        else if (strcmp(command[p->command_lengths[i]-2], "&") != 0)
        {
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
        else if (strcmp(command[p->command_lengths[i]-2], "&") == 0)
        {
            fprintf(stderr, "jsh: syntax error near unexpected token \'|\'\n");
            return -1;
            //spawn_asynchronous_process(command, p->command_lengths[i], inpipe, 
            //        outpipe, appipe, currpipe[0], currpipe[1]);
        }
        else
        {
            fprintf(stderr, "Invalid input.\n");
            return 0;
        }
        /*if ( pipe(currpipe) == -1 )
        {
            fprintf(stderr, "Error: could not create pipe. Aborting command.\n");
            return -1;
        }*/
        /*if (currpipe[0] != -1)
        {
            close(currpipe[0]);
        }
        if (currpipe[1] != -1)
        {
            close(currpipe[1]);
        }
        if (p->num_commands != 1 && i == p->num_commands - 2)
        {
            currpipe[0] = -1;
            currpipe[1] = -1;
        }
        else if (p->num_commands == 1)
        {
            break;
        }
        else
        {
            if ( pipe(currpipe) == -1 )
            {
                fprintf(stderr, "Error: could not create pipe. Aborting command.\n");
                return -1;
            }
        }*/
        if (shouldfree)
        {
            free(command);
        }
    }
    return 0;
}

int _run_async_commands(Piper p)
{
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
