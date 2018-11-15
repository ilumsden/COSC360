#include "spawner.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define PATH_MAX 4096

void cd(char *path)
{
    char *corr_path;
    if (path == NULL || strcmp(path, "~") == 0 || strcmp(path, " ") == 0 || strcmp(path, "") == 0)
    {
        corr_path = getenv("HOME");
        if (corr_path == NULL)
        {
            fprintf(stderr, "The HOME environment variable does not appear to be defined.\n");
            return;
        }
    }
    else
    {
        corr_path = path;
    }
    if (chdir(corr_path) == -1)
    {
        fprintf(stderr, "Could not move to %s\n", path);
    }
}

void spawn_synchronous_process(char **newargv, const char *inpipe, const char *outpipe,
        const char *appendpipe)
{
    int pid, status;
    int fd;
    pid = fork();
    if (pid == 0)
    {
        if (inpipe != NULL)
        {
            fd = open(inpipe, O_RDONLY);
            if (fd < 0)
            {
                fprintf(stderr, "Error: could not open file for input piping. Aborting \
                        command.\n");
                close(fd);
                return;
            }
            dup2(fd, 0);
            close(fd);
        }
        if (outpipe != NULL)
        {
            fd = open(outpipe, O_WRONLY | O_CREAT | O_SYNC | O_TRUNC, 0666);
            if (fd < 0)
            {
                fprintf(stderr, "Error: could not open file for output piping. Aborting \
                        command.\n");
                close(fd);
                return;
            }
            dup2(fd, 1);
            close(fd);
        }
        if (appendpipe != NULL)
        {
            fd = open(outpipe, O_WRONLY | O_CREAT | O_SYNC | O_APPEND, 0666);
            if (fd < 0)
            {
                fprintf(stderr, "Error: could not open file for output piping. Aborting \
                        command.\n");
                close(fd);
                return;
            }
            dup2(fd, 1);
            close(fd);
        }
        if (execvp(newargv[0], newargv) == -1)
        {
            fprintf(stderr, "Error: created new process, but could not launch provided command.\n");
            exit(-2);
        }
    }
    else if (pid == -1)
    {
        fprintf(stderr, "Error: could not run the provided command.\n");
        return;
    }
    else
    {
        wait(&status);
    }
}

void spawn_asynchronous_process(char **newargv, int num_coms, const char *inpipe,
        const char *outpipe, const char *appendpipe)
{
    char **updatedargv = (char**) malloc((num_coms-1)*sizeof(char*));
    for (int i = 0; i < num_coms-1; i++)
    {
        if (i == num_coms-2 && strcmp(newargv[i], "&") != 0)
        {
            fprintf(stderr, "Warning: trying to run an asynchronous process when not requested. \
                    Switching to a synchronous process.\n");
            spawn_synchronous_process(newargv, inpipe, outpipe, appendpipe);
            return;
        }
        else if (i == num_coms-2 && strcmp(newargv[i], "&") == 0)
        {
            updatedargv[i] = NULL;
        }
        else
        {
            updatedargv[i] = newargv[i];
        }
    }
    int pid;
    int fd;
    pid = fork();
    if (pid == 0)
    {
        if (inpipe != NULL)
        {
            fd = open(inpipe, O_RDONLY);
            if (fd < 0)
            {
                fprintf(stderr, "Error: could not open file for input piping. Aborting \
                        command.\n");
                close(fd);
                return;
            }
            dup2(fd, 0);
            close(fd);
        }
        if (outpipe != NULL)
        {
            fd = open(outpipe, O_WRONLY | O_CREAT | O_SYNC | O_TRUNC, 0666);
            if (fd < 0)
            {
                fprintf(stderr, "Error: could not open file for output piping. Aborting \
                        command.\n");
                close(fd);
                return;
            }
            dup2(fd, 1);
            close(fd);
        }
        if (appendpipe != NULL)
        {
            fd = open(outpipe, O_WRONLY | O_CREAT | O_SYNC | O_APPEND, 0666);
            if (fd < 0)
            {
                fprintf(stderr, "Error: could not open file for output piping. Aborting \
                        command.\n");
                close(fd);
                return;
            }
            dup2(fd, 1);
            close(fd);
        }
        if (execvp(updatedargv[0], updatedargv) == -1)
        {
            fprintf(stderr, "Error: created new process, but could not launch provided command.\n");
            free(updatedargv);
            exit(-2);
        }
    }
    else if (pid == -1)
    {
        free(updatedargv);
        fprintf(stderr, "Error: could not run the provided command.\n");
        return;
    }
    else
    {
        free(updatedargv);
        return;
    }
}

int find_input_pipe(char **newargv)
{
    char ** tracker = newargv;
    int ind = 0;
    while (*tracker != NULL)
    {
        if (strcmp(*tracker, "<") == 0)
        {
            return ind;
        }
        ++tracker;
        ind++;
    }
    return -1;
}

int find_output_pipe(char **newargv)
{
    char ** tracker = newargv;
    int ind = 0;
    while (*tracker != NULL)
    {
        if (strcmp(*tracker, ">") == 0)
        {
            return ind;
        }
        ++tracker;
        ind++;
    }
    return -1;
}

int find_append_pipe(char **newargv)
{
    char ** tracker = newargv;
    int ind = 0;
    while (*tracker != NULL)
    {
        if (strcmp(*tracker, ">>") == 0)
        {
            return ind;
        }
        ++tracker;
        ind++;
    }
    return -1;
}
