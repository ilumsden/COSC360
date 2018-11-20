#include "spawner.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#include <errno.h>

#define PATH_MAX 4096

pid_t pidwait(pid_t pid, int *stats)
{
    if (pid < -1 || pid == 0)
    {
        fprintf(stderr, "Invalid PID passed to pidwait.\n");
        return 0;
    }
    while (1)
    {
        pid_t retpid = wait(stats);
        if (pid == -1 || retpid == pid || retpid == (pid_t) -1)
        {
            return retpid;
        }
    }
}

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
        const char *appendpipe, int infd, int outfd, bool firstproc, bool finalproc)
{
    int pid, status;
    int fd;
    pid = fork();
    if (pid == 0)
    {
        if (firstproc)
        {
            close(infd);
        }
        if (finalproc)
        {
            close(outfd);
        }
        if (infd != 0 && !firstproc)
        {
            if (dup2(infd, 0) < 0)
            {
                fprintf(stderr, "Error: could not link file descriptor %d to the new process's stdin.\n", infd);
                exit(-1);
            }
            close(infd);
        }
        if (outfd != 1 && !finalproc)
        {
            if (dup2(outfd, 1) < 0)
            {
                fprintf(stderr, "Error: could not link file descriptor %d to the new process's stdout.\n", infd);
                exit(-1);
            }
            close(outfd);
        }
        if (inpipe != NULL)
        {
            fd = open(inpipe, O_RDONLY);
            if (fd < 0)
            {
                fprintf(stderr, "Error: could not open file for input piping. Aborting command.\n");
                close(fd);
                exit(-1);
            }
            dup2(fd, 0);
            close(fd);
        }
        if (outpipe != NULL)
        {
            fd = open(outpipe, O_WRONLY | O_CREAT | O_SYNC | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if (fd < 0)
            {
                fprintf(stderr, "Error: could not open file for output piping. Aborting command.\n");
                close(fd);
                exit(-1);
            }
            dup2(fd, 1);
            close(fd);
        }
        if (appendpipe != NULL)
        {
            fd = open(appendpipe, O_RDWR | O_CREAT | O_SYNC | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if (fd < 0)
            {
                fprintf(stderr, "Error: could not open file for append piping. Aborting command.\n");
                close(fd);
                exit(-1);
            }
            dup2(fd, 1);
            close(fd);
        }
        if (execvp(newargv[0], newargv) == -1)
        {
            if ( access(newargv[0], F_OK) == -1 )
            {
                perror(newargv[0]);
            }
            else
            {
                fprintf(stderr, "Error: created new process, but could not launch provided command.\n");
            }
            exit(-2);
        }
    }
    else if (pid == -1)
    {
        close(infd);
        close(outfd);
        fprintf(stderr, "Error: could not run the provided command.\n");
        return;
    }
    else
    {
        pidwait(pid, &status);
    }
}

void spawn_asynchronous_process(char **newargv, int num_coms, const char *inpipe,
        const char *outpipe, const char *appendpipe, int infd, int outfd, bool firstproc,
        bool finalproc)
{
    char **updatedargv = (char**) malloc((num_coms-1)*sizeof(char*));
    for (int i = 0; i < num_coms-1; i++)
    {
        if (i == num_coms-2 && strcmp(newargv[i], "&") != 0)
        {
            fprintf(stderr, "Warning: trying to run an asynchronous process when not requested. \
                    Switching to a synchronous process.\n");
            spawn_synchronous_process(newargv, inpipe, outpipe, appendpipe, infd, outfd, firstproc, finalproc);
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
        if (firstproc)
        {
            close(infd);
        }
        if (finalproc)
        {
            close(outfd);
        }
        if (infd != 0 && !firstproc)
        {
            dup2(infd, 0);
            close(infd);
        }
        if (outfd != 1 && !finalproc)
        {
            dup2(outfd, 1);
            close(outfd);
        }
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

int find_input_pipe(char **newargv, int size_newargv)
{
    int ind = 0;
    for (int i = 0; i < size_newargv; i++)
    {
        if (newargv[i] == NULL)
        {
            continue;
        }
        if (strcmp(newargv[i], "<") == 0)
        {
            return ind;
        }
        ind++;
    }
    return -1;
}

int find_output_pipe(char **newargv, int size_newargv)
{
    int ind = 0;
    for (int i = 0; i < size_newargv; i++)
    {
        if (newargv[i] == NULL)
        {
            continue;
        }
        if (strcmp(newargv[i], ">") == 0)
        {
            return ind;
        }
        ind++;
    }
    return -1;
}

int find_append_pipe(char **newargv, int size_newargv)
{
    int ind = 0;
    for (int i = 0; i < size_newargv; i++)
    {
        if (newargv[i] == NULL)
        {
            continue;
        }
        if (strcmp(newargv[i], ">>") == 0)
        {
            return ind;
        }
        ind++;
    }
    return -1;
}

char** remove_single_pipe(char **newargv, int *size_newargv, int pipe_ind)
{
    int ind = 0;
    char **reducedargv = (char**) malloc(((*size_newargv)-2)*sizeof(char*));
    for (int i = 0; i < *size_newargv; i++)
    {
        if (i != pipe_ind && i != pipe_ind+1)
        {
            reducedargv[ind] = newargv[i];
            ind++;
        }
    }
    *size_newargv -= 2;
    free(newargv);
    newargv = reducedargv;
    return reducedargv;
}
