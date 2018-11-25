#include "piper.h"
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
        if (strcmp(argv[1], "-") == 0)
        {
            prompt = strdup("");
        }
        else
        {
            prompt = strdup(argv[1]);
        }
    }
    else
    {
        prompt = strdup("jsh:");
    }
    char buf[MAX_COMMAND_LEN];
    if (strcmp(prompt, "") != 0)
    {
        printf("%s ", prompt);
    }
    while (fgets(buf, MAX_COMMAND_LEN, stdin) != NULL)
    {
        char *input = trim_ws(buf);
        if (strcmp(input, "") == 0)
        {
            if (strcmp(prompt, "") != 0)
            {
                printf("%s ", prompt);
            }
            continue;
        }
        int num_coms = 0;
        char** command = split_command(input, &num_coms);
        Piper p = create_piper(command, num_coms);
        int ret = run_commands(p);
        free_piper(p);
        free(command);
        if (ret == 1)
        {
            break;
        }
        if (strcmp(prompt, "") != 0)
        {
            printf("%s ", prompt);
        }
    }
    free(prompt);
}
