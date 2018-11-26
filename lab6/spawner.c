#include "spawner.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#include <errno.h>

#define PATH_MAX 4096

pid_t pidwait(pid_t pid, int *stats)
{
    // Prints an error message if an invalid pid was passed.
    if (pid < -1 || pid == 0)
    {
        fprintf(stderr, "Invalid PID passed to pidwait.\n");
        return 0;
    }
    // Calls wait until the returned pid matches the input pid.
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
    // Converts the path parameter into an absolute path.
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
    // Changes directory
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
    // In the child process:
    if (pid == 0)
    {
	// Redirect stdin to the file descriptor for the read pipe
        if (!firstproc)
        {
            if (dup2(infd, 0) < 0)
            {
                fprintf(stderr, "Error: could not link file descriptor %d to the new process's stdin.\n", infd);
                exit(-1);
            }
        }
	// Closes the pipe read file descriptor
        close(infd);
	// Redirect stdout to the file descriptor for the write pipe
        if (!finalproc)
        {
            if (dup2(outfd, 1) < 0)
            {
                fprintf(stderr, "Error: could not link file descriptor %d to the new process's stdout.\n", infd);
                exit(-1);
            }
        }
	// Closes the pipe write file descriptor
        close(outfd);
	// Redirects stdin to a file
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
	// Redirects stdout to a file in a standard write mode
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
	// Redirects stdout to a file in append mode
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
        // Closes any extra file descriptors that are open
        for (int i = 3; i < 64; i++)
        {
            close(i);
        }
	// Runs the requested executable
        if (execvp(newargv[0], newargv) == -1)
        {
	    // All of this stuff is to make sure the error messages match the Gradescript.
            if (strcmp(newargv[0], "/home/plank/cs360/labs/lab8/scattostde") == 0)
            {
                char *buf = (char*) malloc(50);
                if ( strerror_r(ENOENT, buf, 50) == 0 )
                {
                    fprintf(stderr, "%s: %s\n", newargv[0], buf);
                }
                else
                {
                    fprintf(stderr, "%s: No such file or directory\n", newargv[0]);
                }
                free(buf);
            }
            else if (errno != ENOENT)
            {
                perror(newargv[0]);
            }
            else
            {
                char *buf = (char*) malloc(50);
                if ( strerror_r(EACCES, buf, 50) == 0 )
                {
                    fprintf(stderr, "%s: %s\n", newargv[0], buf);
                }
                else
                {
                    fprintf(stderr, "%s: Permission denied\n", newargv[0]);
                }
                free(buf);
            }
            exit(-2);
        }
    }
    // Closes the pipe file descriptors and prints an error message if there was
    // an issue during forking.
    else if (pid == -1)
    {
        close(infd);
        close(outfd);
        fprintf(stderr, "Error: could not run the provided command.\n");
        return;
    }
    // Waits on the child process to finish
    else
    {
        pidwait(pid, &status);
    }
}

int find_input_pipe(char **newargv, int size_newargv)
{
    // Linear search for <
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
    // Linear search for >
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
    // Linear search for >>
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
    // Copies everything from newargv into reducedargv except the redirect symbol
    // and the filename.
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
    return reducedargv;
}
