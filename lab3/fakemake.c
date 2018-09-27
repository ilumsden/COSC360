#include "compile.h"
#include "fields.h"

char* recover_fname_from_command(char *command)
{
    int fnamelen = 0;
    int startidx = -1;
    for (int i = strlen(command)-1; i >= 0; i--)
    {
        if (command[i] == ' ')
        {
            startidx = i+1;
            fnamelen = strlen(command) - startidx;
            break;
        }
    }
    if (startidx == -1)
    {
        return command;
    }
    char *fname = (char*) malloc(fnamelen+1);
    strcpy(fname, &command[startidx]);
    return fname;
}

int main(int argc, char **argv)
{
    char target[50] = "fmakefile";
    //printf("target is %s\n", target);
    if (argc == 2)
    {
        strcpy(target, argv[1]);
    }
    IS is = new_inputstruct(target);
    Compile *compilation = new_compilation();
    while (get_line(is) >= 0)
    {
        if (is->NF == 0)
        {
            continue;
        }
        else if (strcmp(is->fields[0], "E") == 0)
        {
            add_exec(compilation, is->fields, is->NF, is->line);
        }
        else if (strcmp(is->fields[0], "C") == 0)
        {
            add_srcs(compilation, is->fields, is->NF);
        }
        else if (strcmp(is->fields[0], "H") == 0)
        {
            add_headers(compilation, is->fields, is->NF);
        }
        else if (strcmp(is->fields[0], "F") == 0)
        {
            add_flags(compilation, is->fields, is->NF);
        }
        else if (strcmp(is->fields[0], "L") == 0)
        {
            add_libraries(compilation, is->fields, is->NF);
        }
        else
        {
            fprintf(stderr, "Error: invalid line (%d)\n", is->line);
            return -1;
        }
    }
    jettison_inputstruct(is);
    if (compilation->exec_name == NULL || strcmp(compilation->exec_name, "") == 0)
    {
        fprintf(stderr, "No executable specified\n");
        free_compilation(compilation);
        return -1;
    }
    char **commands = get_commands(compilation);
    for (int i = 0; i < compilation->num_commands-1; i++)
    {
        char *fname = recover_fname_from_command(commands[i]);
        if ( access(fname, F_OK) != 0 )
        {
            fprintf(stderr, "fmakefile: %s: No such file or directory\n", fname);
            free(fname);
            for (int i = 0; i < compilation->num_commands; i++)
            {
                free(commands[i]);
            }
            free(commands);
            free_compilation(compilation);
            return -1;
        }
        free(fname);
        printf("%s\n", commands[i]);
        if ( system(commands[i]) != 0 )
        {
            fprintf(stderr, "Command failed.  Exiting\n");
            for (int i = 0; i < compilation->num_commands; i++)
            {
                free(commands[i]);
            }
            free(commands);
            free_compilation(compilation);
            return -1;
        }
    }
    printf("%s\n", commands[compilation->num_commands-1]);
    if ( system(commands[compilation->num_commands-1]) != 0 )
    {
        fprintf(stderr, "Command failed.  Fakemake exiting\n");
        return -1;
    }
    for (int i = 0; i < compilation->num_commands; i++)
    {
        free(commands[i]);
    }
    free(commands);
    free_compilation(compilation);
    return 0;
}
