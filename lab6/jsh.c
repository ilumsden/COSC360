#include "spawner.h"
#include <ctype.h>

#define MAX_COMMAND_LEN 2048

char* trim_ws(char* input)
{
    while (isspace((unsigned char) *input)) {++input;}
    if (*input == 0)
    {
        return input;
    }
    char *end = input + strlen(input) - 1;
    while (end > input && isspace((unsigned char) *end)) {--end;}
    end[1] = 0;
    return input;
}

char** split_command(char* input, int *num_spaces)
{
    char **args = NULL;
    char *p = strtok(input, " \n");
    *num_spaces = 0;
    while (p)
    {
        args = (char**) realloc(args, ++(*num_spaces)*sizeof(char*));
        if (args == NULL)
        {
            fprintf(stderr, "Error: could not allocate memory for input splitting.\n");
            exit(-1);
        }
        args[*(num_spaces)-1] = p;
        p = strtok(NULL, " \n");
    }
    args = (char**) realloc(args, ++(*num_spaces)*sizeof(char*));
    if (args == NULL)
    {
        fprintf(stderr, "Error: could not allocate memory for input splitting.\n");
        exit(-1);
    }
    args[(*num_spaces)-1] = NULL;
    return args;
}

int main(int argc, char **argv)
{
    char *prompt;
    if (argc == 2)
    {
        prompt = strdup(argv[1]);
    }
    else
    {
        prompt = strdup("jsh:");
    }
    char buf[MAX_COMMAND_LEN];
    printf("%s ", prompt);
    while (fgets(buf, MAX_COMMAND_LEN, stdin) != NULL)
    {
        char *input = trim_ws(buf);
        if (strcmp(input, "") == 0)
        {
            printf("%s ", prompt);
            continue;
        }
        int num_coms = 0;
        char** command = split_command(input, &num_coms);
        if (strcmp(command[0], "cd") == 0)
        {
            printf("%s ", prompt);
            cd(command[1]);
            continue;
        }
        char *inpipe = NULL;
        char *outpipe = NULL;
        char *appipe = NULL;
        int pind = find_input_pipe(command);
        if (pind != -1)
        {
            inpipe = command[pind+1];
        }
        pind = find_output_pipe(command);
        if (pind != -1)
        {
            outpipe = command[pind+1];
        }
        pind = find_append_pipe(command);
        if (pind != -1)
        {
            appipe = command[pind+1];
        }
        if (strcmp(command[0], "exit") == 0)
        {
            break;
        }
        else if (strcmp(command[num_coms-2], "&") != 0)
        {
            spawn_synchronous_process(command, inpipe, outpipe, appipe);
        }
        else if (strcmp(command[num_coms-2], "&") == 0)
        {
            spawn_asynchronous_process(command, num_coms, inpipe, outpipe, appipe);
        }
        else
        {
            fprintf(stderr, "Invalid input.\n");
        }
        printf("%s ", prompt);
    }
    free(prompt);
}
